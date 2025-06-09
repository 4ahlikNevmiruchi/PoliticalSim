#include <QString>
#include <QFile>
#include <QDebug>

/**
 * @brief Automatically deletes the specified file when the object goes out of scope.
 *        Use this for test database cleanup.
 */
struct ScopedFileRemover {
    QString path;

    explicit ScopedFileRemover(const QString& filePath)
        : path(filePath) {}

    ~ScopedFileRemover() {
        if (QFile::exists(path)) {
            bool success = QFile::remove(path);
            if (!success)
                qWarning() << "Failed to delete file:" << path;
        }
    }
};