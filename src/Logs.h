#pragma once

#include <heltec.h>
#include <map>
#include <vector>

class Logs {
public:
  explicit Logs(size_t maxLogSize = 4096) : _maxLogSize(maxLogSize) {}

  void clear() { _logs.clear(); }

  // Ajouter un log avec un niveau
  void addLog(const String& level, const String& message) {
    if (_logs[level].size() >= _maxLogSize) {
      // Si la capacité est dépassée, on supprime la première entrée
      _logs[level].erase(_logs[level].begin());
    }
    _logs[level].push_back(message);
    Serial.println(message);
  }

  // Ajouter un log formaté
  void addLogf(const String& level, const char* fmt, ...) {
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    addLog(level, String(buffer));
    Serial.println(buffer);
  }

  // Récupérer les logs pour un niveau donné
  String getLogsByLevel(const String& level) {
    String result = "";
    for (const auto& log : _logs[level]) {
      result += log + "\n";
    }
    return result;
  }

  // Récupérer tous les logs
  String getAllLogs() {
    String result = "";
    for (const auto& pair : _logs) {
      result += "Level: " + pair.first + "\n";
      for (const auto& log : pair.second) {
        result += log + "\n";
      }
    }
    return result;
  }

  size_t getLogCount(const String& level) {
    return _logs[level].size();
  }

private:
  size_t _maxLogSize;
  std::map<String, std::vector<String>> _logs;  // Associe le niveau de log à une liste de messages
};

extern Logs logs;
