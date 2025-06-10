#include <QString>
#include <QFile>
#include <QDebug>

/**
 * @brief RAII utility that deletes a specified file upon destruction.
 *
 * Primarily used to clean up temporary files (e.g., test database files).
 */
struct ScopedFileRemover {
    QString path;    ///< Path of the file to remove when this object is destroyed.

    /**
     * @brief Constructs a ScopedFileRemover for the given file path.
     * @param filePath Path to the file that will be removed when this object is destroyed.
     */
    explicit ScopedFileRemover(const QString& filePath)
        : path(filePath) {}

    /** @brief Destructor. Removes the file if it still exists. */
    ~ScopedFileRemover() {
        if (QFile::exists(path)) {
            bool success = QFile::remove(path);
            if (!success)
                qWarning() << "Failed to delete file:" << path;
        }
    }
};
