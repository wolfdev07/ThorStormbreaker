// ThorStormbreaker.cpp: define el punto de entrada de la aplicación.
//
#include "services/implement/FingerprintServiceImpl.h"

#define NOMINMAX  // Evitar conflicto con std::numeric_limits
#include <windows.h>

#include <memory>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <limits>

using namespace std;

// ============================================
// Estructuras de datos
// ============================================

struct Usuario {
    unsigned int id;
    string nombre;
    vector<unsigned char> templateHuella;
};

// Base de datos de usuarios (en memoria)
map<unsigned int, Usuario> g_usuarios;
unsigned int g_nextId = 1;

// ============================================
// Funciones auxiliares
// ============================================

void limpiarConsola() {
    system("cls");
}

void pausar() {
    cout << "\nPresione ENTER para continuar...";
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    cin.get();
}

void mostrarEncabezado(const string& titulo) {
    cout << "\n============================================" << endl;
    cout << titulo << endl;
    cout << "============================================\n" << endl;
}

bool capturarHuella(IFingerprintService* service, vector<unsigned char>& fpImage, vector<unsigned char>& fpTemplate) {
    cout << "\n>> Coloque el dedo en el lector..." << endl;

    int intentos = 0;
    const int MAX_INTENTOS = 50; // 5 segundos

    while (intentos < MAX_INTENTOS) {
        if (service->acquireFingerprint(fpImage, fpTemplate)) {
            cout << "   [OK] Huella capturada!" << endl;
            cout << "   Tamaño de imagen: " << fpImage.size() << " bytes" << endl;
            cout << "   Tamaño de template: " << fpTemplate.size() << " bytes" << endl;
            return true;
        }

        Sleep(100);
        intentos++;
    }

    cout << "   [TIMEOUT] No se detectó huella en 5 segundos" << endl;
    return false;
}

// ============================================
// Funcionalidades principales
// ============================================

void capturaSimple(IFingerprintService* service) {
    mostrarEncabezado("CAPTURA SIMPLE DE HUELLA");

    vector<unsigned char> fpImage, fpTemplate;

    if (capturarHuella(service, fpImage, fpTemplate)) {
        cout << "\n¡Captura exitosa!" << endl;
        cout << "Esta huella no se guardó, solo fue una prueba." << endl;
    }
    else {
        cout << "\nError al capturar huella" << endl;
    }

    pausar();
}

void registrarUsuario(IFingerprintService* service) {
    mostrarEncabezado("REGISTRO DE NUEVO USUARIO");

    string nombre;
    cout << "Ingrese el nombre del usuario: ";
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    getline(cin, nombre);

    if (nombre.empty()) {
        cout << "\nNombre invalido" << endl;
        pausar();
        return;
    }

    cout << "\nSe capturara la misma huella 3 veces para mayor precision" << endl;

    vector<vector<unsigned char>> templates;
    vector<unsigned char> fpImage, fpTemplate;

    // Capturar 3 veces
    for (int i = 0; i < 3; i++) {
        cout << "\n--- Captura " << (i + 1) << "/3 ---" << endl;

        if (!capturarHuella(service, fpImage, fpTemplate)) {
            cout << "\nError: No se pudo completar el registro" << endl;
            pausar();
            return;
        }

        // Verificar que sea la misma huella (solo después de la primera captura)
        if (i > 0) {
            int score;
            if (!service->matchTemplates(templates[0], fpTemplate, score)) {
                cout << "\n[ERROR] No se pudo comparar las huellas" << endl;
                pausar();
                return;
            }

            cout << "   Score de similitud con la 1ra captura: " << score << endl;

            if (score < 50) {
                cout << "\n[ERROR] Las huellas no coinciden!" << endl;
                cout << "Asegurese de usar el MISMO dedo en las 3 capturas" << endl;
                pausar();
                return;
            }
        }

        templates.push_back(fpTemplate);

        if (i < 2) {
            cout << "\nRetire el dedo y vuelva a colocarlo..." << endl;
            Sleep(1500);
        }
    }

    // Fusionar las 3 capturas
    cout << "\n>> Fusionando templates..." << endl;
    vector<unsigned char> regTemplate;

    if (!service->mergeTemplates(templates[0], templates[1], templates[2], regTemplate)) {
        cout << "[ERROR] No se pudo fusionar los templates" << endl;
        pausar();
        return;
    }

    cout << "   [OK] Templates fusionados" << endl;
    cout << "   Tamaño del template final: " << regTemplate.size() << " bytes" << endl;

    // Agregar a la base de datos
    unsigned int userId = g_nextId++;

    if (!service->addTemplate(userId, regTemplate)) {
        cout << "[ERROR] No se pudo agregar a la base de datos" << endl;
        pausar();
        return;
    }

    // Guardar en nuestra estructura
    Usuario usuario;
    usuario.id = userId;
    usuario.nombre = nombre;
    usuario.templateHuella = regTemplate;
    g_usuarios[userId] = usuario;

    mostrarEncabezado("REGISTRO EXITOSO");
    cout << "Usuario ID  : " << userId << endl;
    cout << "Nombre      : " << nombre << endl;
    cout << "Template    : " << regTemplate.size() << " bytes" << endl;
    cout << "\nTotal usuarios registrados: " << g_usuarios.size() << endl;

    pausar();
}

void identificarUsuario(IFingerprintService* service) {
    mostrarEncabezado("IDENTIFICACION DE USUARIO (1:N)");

    if (g_usuarios.empty()) {
        cout << "No hay usuarios registrados todavia" << endl;
        cout << "Registre al menos un usuario primero" << endl;
        pausar();
        return;
    }

    cout << "Usuarios en la base de datos: " << g_usuarios.size() << endl;

    vector<unsigned char> fpImage, fpTemplate;

    if (!capturarHuella(service, fpImage, fpTemplate)) {
        cout << "\nError al capturar huella" << endl;
        pausar();
        return;
    }

    // Identificar
    cout << "\n>> Buscando en la base de datos..." << endl;
    unsigned int fid = 0;
    unsigned int score = 0;

    if (service->identify(fpTemplate, fid, score)) {
        // Buscar en nuestra estructura
        auto it = g_usuarios.find(fid);
        if (it != g_usuarios.end()) {
            mostrarEncabezado("USUARIO IDENTIFICADO");
            cout << "ID          : " << it->second.id << endl;
            cout << "Nombre      : " << it->second.nombre << endl;
            cout << "Score       : " << score << endl;
            cout << "Confianza   : " << (score > 80 ? "MUY ALTA" : score > 60 ? "ALTA" : "MEDIA") << endl;
        }
        else {
            cout << "\n[ERROR] Usuario encontrado en BD pero no en memoria (ID: " << fid << ")" << endl;
        }
    }
    else {
        cout << "\n[NO IDENTIFICADO]" << endl;
        cout << "Esta huella no esta registrada en el sistema" << endl;
    }

    pausar();
}

void verificarUsuario(IFingerprintService* service) {
    mostrarEncabezado("VERIFICACION DE USUARIO (1:1)");

    if (g_usuarios.empty()) {
        cout << "No hay usuarios registrados todavia" << endl;
        pausar();
        return;
    }

    // Mostrar usuarios
    cout << "Usuarios registrados:" << endl;
    for (const auto& pair : g_usuarios) {
        cout << "  [" << pair.second.id << "] " << pair.second.nombre << endl;
    }

    cout << "\nIngrese el ID del usuario a verificar: ";
    unsigned int userId;
    cin >> userId;

    auto it = g_usuarios.find(userId);
    if (it == g_usuarios.end()) {
        cout << "\nUsuario no encontrado" << endl;
        pausar();
        return;
    }

    cout << "\nVerificando identidad de: " << it->second.nombre << endl;

    vector<unsigned char> fpImage, fpTemplate;

    if (!capturarHuella(service, fpImage, fpTemplate)) {
        cout << "\nError al capturar huella" << endl;
        pausar();
        return;
    }

    // Comparar
    cout << "\n>> Comparando huellas..." << endl;
    int score;

    if (service->matchTemplates(it->second.templateHuella, fpTemplate, score)) {
        mostrarEncabezado("RESULTADO DE VERIFICACION");
        cout << "Usuario     : " << it->second.nombre << " (ID: " << userId << ")" << endl;
        cout << "Score       : " << score << endl;

        if (score > 60) {
            cout << "Estado      : VERIFICADO ✓" << endl;
            cout << "Confianza   : " << (score > 80 ? "MUY ALTA" : "ALTA") << endl;
        }
        else {
            cout << "Estado      : NO VERIFICADO ✗" << endl;
            cout << "Confianza   : BAJA (posible impostor)" << endl;
        }
    }
    else {
        cout << "\n[ERROR] No se pudo comparar las huellas" << endl;
    }

    pausar();
}

void listarUsuarios() {
    mostrarEncabezado("LISTA DE USUARIOS REGISTRADOS");

    if (g_usuarios.empty()) {
        cout << "No hay usuarios registrados" << endl;
    }
    else {
        cout << left << setw(5) << "ID" << setw(30) << "Nombre" << setw(15) << "Template" << endl;
        cout << string(50, '-') << endl;

        for (const auto& pair : g_usuarios) {
            cout << left
                << setw(5) << pair.second.id
                << setw(30) << pair.second.nombre
                << setw(15) << (to_string(pair.second.templateHuella.size()) + " bytes")
                << endl;
        }

        cout << "\nTotal: " << g_usuarios.size() << " usuario(s)" << endl;
    }

    pausar();
}

void eliminarUsuario(IFingerprintService* service) {
    mostrarEncabezado("ELIMINAR USUARIO");

    if (g_usuarios.empty()) {
        cout << "No hay usuarios registrados" << endl;
        pausar();
        return;
    }

    // Mostrar usuarios
    cout << "Usuarios registrados:" << endl;
    for (const auto& pair : g_usuarios) {
        cout << "  [" << pair.second.id << "] " << pair.second.nombre << endl;
    }

    cout << "\nIngrese el ID del usuario a eliminar (0 para cancelar): ";
    unsigned int userId;
    cin >> userId;

    if (userId == 0) {
        return;
    }

    auto it = g_usuarios.find(userId);
    if (it == g_usuarios.end()) {
        cout << "\nUsuario no encontrado" << endl;
        pausar();
        return;
    }

    string nombre = it->second.nombre;

    // Confirmar
    cout << "\n¿Esta seguro de eliminar a '" << nombre << "'? (s/n): ";
    char confirm;
    cin >> confirm;

    if (confirm == 's' || confirm == 'S') {
        if (service->deleteTemplate(userId)) {
            g_usuarios.erase(it);
            cout << "\n[OK] Usuario eliminado correctamente" << endl;
        }
        else {
            cout << "\n[ERROR] No se pudo eliminar de la base de datos" << endl;
        }
    }
    else {
        cout << "\nOperacion cancelada" << endl;
    }

    pausar();
}

void mostrarMenu() {
    limpiarConsola();
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║    ThorStormbreaker - Sistema de Huellas      ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    cout << "\n  1. Captura simple de huella" << endl;
    cout << "  2. Registrar nuevo usuario" << endl;
    cout << "  3. Identificar usuario (1:N)" << endl;
    cout << "  4. Verificar usuario (1:1)" << endl;
    cout << "  5. Listar usuarios registrados" << endl;
    cout << "  6. Eliminar usuario" << endl;
    cout << "  0. Salir" << endl;
    cout << "\n  Usuarios registrados: " << g_usuarios.size() << endl;
    cout << "\n  Seleccione una opcion: ";
}


int main() {
    // Inicialización del sistema
    limpiarConsola();
    cout << "============================================" << endl;
    cout << "ThorStormbreaker - Sistema de Huellas v1.0" << endl;
    cout << "============================================" << endl << endl;

    cout << "[1/3] Creando servicio de huellas..." << endl;
    unique_ptr<IFingerprintService> fingerprintService =
        make_unique<FingerprintServiceImpl>();

    cout << "[2/3] Inicializando SDK..." << endl;
    if (!fingerprintService->initialize()) {
        cerr << "\nERROR: No se pudo inicializar el SDK" << endl;
        cerr << "Asegurese de que el dispositivo este conectado" << endl;
        pausar();
        return -1;
    }

    cout << "[3/3] Abriendo dispositivo..." << endl;
    int deviceCount = fingerprintService->getDeviceCount();

    if (deviceCount <= 0) {
        cerr << "\nERROR: No se encontraron dispositivos" << endl;
        fingerprintService->terminate();
        pausar();
        return -1;
    }

    if (!fingerprintService->openDevice(0)) {
        cerr << "\nERROR: No se pudo abrir el dispositivo" << endl;
        fingerprintService->terminate();
        pausar();
        return -1;
    }

    if (!fingerprintService->initDatabase()) {
        cerr << "\nERROR: No se pudo inicializar la base de datos" << endl;
        fingerprintService->closeDevice();
        fingerprintService->terminate();
        pausar();
        return -1;
    }

    int width, height;
    fingerprintService->getImageDimensions(width, height);

    cout << "\n[OK] Sistema inicializado correctamente" << endl;
    cout << "     Dispositivos: " << deviceCount << endl;
    cout << "     Resolucion: " << width << "x" << height << " px" << endl;

    Sleep(1500);

    // Menú principal
    int opcion;
    do {
        mostrarMenu();
        cin >> opcion;

        if (cin.fail()) {
            cin.clear();
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            opcion = -1;
        }

        switch (opcion) {
        case 1:
            capturaSimple(fingerprintService.get());
            break;
        case 2:
            registrarUsuario(fingerprintService.get());
            break;
        case 3:
            identificarUsuario(fingerprintService.get());
            break;
        case 4:
            verificarUsuario(fingerprintService.get());
            break;
        case 5:
            listarUsuarios();
            break;
        case 6:
            eliminarUsuario(fingerprintService.get());
            break;
        case 0:
            cout << "\nCerrando sistema..." << endl;
            break;
        default:
            cout << "\nOpcion invalida" << endl;
            Sleep(1000);
            break;
        }
    } while (opcion != 0);

    // Limpieza
    fingerprintService->freeDatabase();
    fingerprintService->closeDevice();
    fingerprintService->terminate();

    cout << "\nGracias por usar ThorStormbreaker" << endl;
    return 0;
}
