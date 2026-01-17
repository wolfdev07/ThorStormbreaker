//
// Created by artur on 17/01/2026.
//
#include <cstdint>
#include <sqlite3.h>
#include <vector>
#include <string>
#include <ctime>
#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

struct FingerPrint {
    int id{};
    std::string uid;
    std::vector<uint8_t> templateData;
    std::string createdAt;
};

class FingerPrintRepository {
private:
    sqlite3* db{};

    static std::string now() {
        time_t t = time(nullptr);
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
        return std::string(buf);
    }

    void exec(const std::string& sql) {
        char* err = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
            std::string msg = err;
            sqlite3_free(err);
            throw std::runtime_error(msg);
        }
    }

public:
    explicit FingerPrintRepository(const std::string& db_path) {
        fs::path p(db_path);
        fs::create_directories(p.parent_path()); // ← CLAVE

        if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
            throw std::runtime_error("Cannot open database");
        }

        exec(
            "CREATE TABLE IF NOT EXISTS fingerprints ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "uid TEXT UNIQUE NOT NULL,"
            "template BLOB NOT NULL,"
            "created_at TEXT NOT NULL"
            ");"
        );
    }

    ~FingerPrintRepository() {
        sqlite3_close(db);
    }

    [[nodiscard]] FingerPrint saveFingerPrint(const std::string& uid, const std::vector<uint8_t>& tpl) const {
        const auto sql =
            "INSERT INTO fingerprints (uid, template, created_at) "
            "VALUES (?, ?, ?);";

        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, uid.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_blob(stmt, 2, tpl.data(), static_cast<int>(tpl.size()), SQLITE_STATIC);
        std::string created = now();
        sqlite3_bind_text(stmt, 3, created.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Error inserting fingerprint");
        }

        sqlite3_finalize(stmt);

        FingerPrint fp;
        fp.id = static_cast<int>(sqlite3_last_insert_rowid(db));
        fp.uid = uid;
        fp.templateData = tpl;
        fp.createdAt = created;

        return fp;
    }

    std::vector<FingerPrint> getAll() {
        std::vector<FingerPrint> list;
        const auto sql = "SELECT id, uid, template, created_at FROM fingerprints;";

        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            FingerPrint fp;
            fp.id = sqlite3_column_int(stmt, 0);
            fp.uid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

            const auto* blob =
                static_cast<const uint8_t*>(sqlite3_column_blob(stmt, 2));
            int size = sqlite3_column_bytes(stmt, 2);
            fp.templateData.assign(blob, blob + size);

            fp.createdAt =
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

            list.push_back(fp);
        }
        sqlite3_finalize(stmt);
        return list;
    }

    FingerPrint getById(int id) {
        const auto sql = "SELECT id, uid, template, created_at FROM fingerprints WHERE id=?;";

        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) != SQLITE_ROW) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Fingerprint not found");
        }

        FingerPrint fp;
        fp.id = sqlite3_column_int(stmt, 0);
        fp.uid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        const auto* blob =
            static_cast<const uint8_t*>(sqlite3_column_blob(stmt, 2));
        int size = sqlite3_column_bytes(stmt, 2);
        fp.templateData.assign(blob, blob + size);

        fp.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        sqlite3_finalize(stmt);
        return fp;
    }
};