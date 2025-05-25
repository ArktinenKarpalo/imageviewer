#pragma once
#include <QImage>
#include <optional>

namespace utils {
std::optional<QImage> loadImage(const QString &filename);
};
