// Приклади рефакторингу C++ (lab1 ParP): три методи Фаулера
// Андренко Анна Сергіївна, ПЗПІ-24-5. Практичне завдання № 2.
//
// Файл містить код до і після рефакторингу у просторах імен:
//   m1_before / m1_after - Convert Procedural Design to Objects (MatrixTiming)
//   m2_before / m2_after - Replace Type Code with Subclasses (TimerResult)
//   m3_before / m3_after - Replace Record with Data Class (SavedRow)
//
// Збірка: g++ -std=c++14 -Wall -Wextra code-examples-refactoring-methods.cpp -o demo
// Запуск: ./demo before > before.txt ; ./demo after > after.txt ; diff before.txt after.txt
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

static std::string buildMode() { return "Release"; }
// Скидає формат виведення (fixed, precision), змінений тестом методу 1
static void resetFormat() { std::cout.copyfmt(std::ios(nullptr)); }

namespace m1_before {

struct MatrixTiming {
    std::size_t n;
    std::string type;
    double rawSec;
    double objectsSec;
    int repetitions;
    bool equal;
    long double sum;
};
void saveMatrixResult(const MatrixTiming& r) {
    const char* filename = "lab1_matrix_results.csv";
    bool isEmpty = true;
    {
        std::ifstream in(filename, std::ios::binary | std::ios::ate);
        if (in && in.tellg() > 0) isEmpty = false;
    }
    std::ofstream out(filename, std::ios::app);
    if (!out) {
        std::cerr << "Could not write " << filename << '\n';
        return;
    }
    if (isEmpty)
        out << "mode,type,n,n_cubed,raw_seconds,object_seconds,object_over_raw,repetitions,equal,checksum\n";
    const std::uint64_t n3 = static_cast<std::uint64_t>(r.n) * r.n * r.n;
    out << buildMode() << ',' << r.type << ',' << r.n << ',' << n3 << ','
        << std::setprecision(14) << r.rawSec << ',' << r.objectsSec << ','
        << (r.rawSec > 0 ? r.objectsSec / r.rawSec : 0.0) << ','
        << r.repetitions << ',' << (r.equal ? "YES" : "NO") << ','
        << r.sum << '\n';
}
void displayMatrixResult(const MatrixTiming& r) {
    std::cout << "\nMode: " << buildMode() << ", type: " << r.type << ", N=" << r.n
              << ", repetitions=" << r.repetitions << '\n'
              << std::fixed << std::setprecision(6)
              << "Raw arrays:      " << r.rawSec << " s\n"
              << "Matrix objects:  " << r.objectsSec << " s\n"
              << "Object/raw ratio: " << (r.rawSec > 0 ? r.objectsSec / r.rawSec : 0.0) << '\n'
              << "Results identical: " << (r.equal ? "YES" : "NO") << '\n'
              << "Checksum: " << std::setprecision(0) << r.sum << '\n';
    saveMatrixResult(r);
    if (!r.equal)
        throw std::runtime_error("Raw/object results are different");
}

// --- тест m1_before ---
void test() {
    try { displayMatrixResult(MatrixTiming{512, "int32", 0.123456789, 0.246913578, 3, true, 262144.0L}); }
    catch (const std::exception& e) { std::cout << "EXC: " << e.what() << '\n'; }
    try { displayMatrixResult(MatrixTiming{1024, "double", 0.0, 1.5, 2, true, 524288.0L}); }
    catch (const std::exception& e) { std::cout << "EXC: " << e.what() << '\n'; }
    try { displayMatrixResult(MatrixTiming{2048, "int8", 3.25, 3.5, 1, false, 99.0L}); }
    catch (const std::exception& e) { std::cout << "EXC: " << e.what() << '\n'; }
}

}  // namespace m1_before

namespace m1_after {

class MatrixTiming {
public:
    MatrixTiming(std::size_t n, std::string type,
                 double rawSec, double objectsSec,
                 int repetitions, bool equal, long double sum)
        : n_(n), type_(std::move(type)), rawSec_(rawSec),
          objectsSec_(objectsSec), repetitions_(repetitions),
          equal_(equal), sum_(sum) {}

    bool equal() const { return equal_; }
    double ratio() const {
        return rawSec_ > 0 ? objectsSec_ / rawSec_ : 0.0;
    }
    std::uint64_t nCubed() const {
        return static_cast<std::uint64_t>(n_) * n_ * n_;
    }
    void save() const;
    void display() const;

private:
    std::size_t n_;
    std::string type_;
    double rawSec_;
    double objectsSec_;
    int repetitions_;
    bool equal_;
    long double sum_;
};

void MatrixTiming::save() const {
    const char* filename = "lab1_matrix_results.csv";
    bool isEmpty = true;
    {
        std::ifstream in(filename, std::ios::binary | std::ios::ate);
        if (in && in.tellg() > 0) isEmpty = false;
    }
    std::ofstream out(filename, std::ios::app);
    if (!out) {
        std::cerr << "Could not write " << filename << '\n';
        return;
    }
    if (isEmpty)
        out << "mode,type,n,n_cubed,raw_seconds,object_seconds,object_over_raw,repetitions,equal,checksum\n";
    out << buildMode() << ',' << type_ << ',' << n_ << ',' << nCubed() << ','
        << std::setprecision(14) << rawSec_ << ',' << objectsSec_ << ','
        << ratio() << ','
        << repetitions_ << ',' << (equal_ ? "YES" : "NO") << ','
        << sum_ << '\n';
}

void MatrixTiming::display() const {
    std::cout << "\nMode: " << buildMode() << ", type: " << type_ << ", N=" << n_
              << ", repetitions=" << repetitions_ << '\n'
              << std::fixed << std::setprecision(6)
              << "Raw arrays:      " << rawSec_ << " s\n"
              << "Matrix objects:  " << objectsSec_ << " s\n"
              << "Object/raw ratio: " << ratio() << '\n'
              << "Results identical: " << (equal_ ? "YES" : "NO") << '\n'
              << "Checksum: " << std::setprecision(0) << sum_ << '\n';
    save();
    if (!equal_)
        throw std::runtime_error("Raw/object results are different");
}

// --- тест m1_after ---
void test() {
    try { MatrixTiming(512, "int32", 0.123456789, 0.246913578, 3, true, 262144.0L).display(); }
    catch (const std::exception& e) { std::cout << "EXC: " << e.what() << '\n'; }
    try { MatrixTiming(1024, "double", 0.0, 1.5, 2, true, 524288.0L).display(); }
    catch (const std::exception& e) { std::cout << "EXC: " << e.what() << '\n'; }
    try { MatrixTiming(2048, "int8", 3.25, 3.5, 1, false, 99.0L).display(); }
    catch (const std::exception& e) { std::cout << "EXC: " << e.what() << '\n'; }
}

}  // namespace m1_after

namespace m2_before {

struct TimerResult {
    const char* name;
    const char* header;
    const char* unit;
    long double minStep;
    long double inNanoseconds;
};
static long double freqG;
static long double nsOf(const TimerResult& e) {
    const long double freq = freqG;
    long double ns = -1;
    if (e.minStep >= 0) {
        const std::string name = e.name;
        if (name == "time" || name == "omp_get_wtime")
            ns = e.minStep * 1.0e9L;
        else if (name == "clock")
            ns = e.minStep * 1.0e9L / CLOCKS_PER_SEC;
        else if (name == "GetSystemTimeAsFileTime" || name == "GetSystemTimePreciseAsFileTime")
            ns = e.minStep * 100.0L;
        else if (name == "GetTickCount")
            ns = e.minStep * 1.0e6L;
        else if (name == "QueryPerformanceCounter")
            ns = e.minStep * 1.0e9L / freq;
        else if (name == "chrono::high_resolution_clock")
            ns = e.minStep;
    }
    return ns;
}
static void run(long double freq, long double s) {
    freqG = freq;
    const std::vector<TimerResult> r = {
        {"time", "time.h", "seconds", s, 0},
        {"clock", "time.h", "clock ticks", s, 0},
        {"GetSystemTimeAsFileTime", "windows.h", "100 ns ticks", s, 0},
        {"GetSystemTimePreciseAsFileTime", "windows.h", "100 ns ticks", s, 0},
        {"GetTickCount", "windows.h", "milliseconds", s, 0},
        {"__rdtsc", "intrin.h", "TSC ticks/cycles", s, 0},
        {"QueryPerformanceCounter", "windows.h", "QPC ticks", s, 0},
        {"chrono::high_resolution_clock", "chrono", "nanoseconds", s, 0},
        {"omp_get_wtime", "omp.h", "seconds", s, 0}};
    for (auto e : r)
        std::printf("%s|%s|%s|%.6f|%.9Lf\n", e.name, e.header, e.unit, static_cast<double>(e.minStep), nsOf(e));
}

// --- тест m2_before ---
void test() {
    long double steps[] = {-1.0L, 0.0L, 1.0L, 15.625L, 0.0001L, 1000.0L};
    for (long double s : steps) run(10000000.0L, s);
    run(3000000.0L, 2.5L);
}

}  // namespace m2_before

namespace m2_after {

class TimerResult {
public:
    TimerResult(const char* name, const char* header,
                const char* unit, long double minStep)
        : name_(name), header_(header), unit_(unit), minStep_(minStep) {}
    virtual ~TimerResult() = default;
    const char* name() const { return name_; }
    const char* header() const { return header_; }
    const char* unit() const { return unit_; }
    long double minStep() const { return minStep_; }
    long double inNanoseconds() const {
        return minStep_ >= 0 ? convert(minStep_) : -1;
    }
protected:
    virtual long double convert(long double step) const = 0;
private:
    const char* name_;
    const char* header_;
    const char* unit_;
    long double minStep_;
};

class SecondsTimer : public TimerResult {
    using TimerResult::TimerResult;
    long double convert(long double s) const override { return s * 1.0e9L; }
};
class ClockTimer : public TimerResult {
    using TimerResult::TimerResult;
    long double convert(long double s) const override { return s * 1.0e9L / CLOCKS_PER_SEC; }
};
class FileTimeTimer : public TimerResult {
    using TimerResult::TimerResult;
    long double convert(long double s) const override { return s * 100.0L; }
};
class TickCountTimer : public TimerResult {
    using TimerResult::TimerResult;
    long double convert(long double s) const override { return s * 1.0e6L; }
};
class QpcTimer : public TimerResult {
public:
    QpcTimer(const char* name, const char* header, const char* unit,
             long double minStep, long double freq)
        : TimerResult(name, header, unit, minStep), freq_(freq) {}
protected:
    long double convert(long double s) const override { return s * 1.0e9L / freq_; }
private:
    long double freq_;
};
class ChronoTimer : public TimerResult {
    using TimerResult::TimerResult;
    long double convert(long double s) const override { return s; }
};
class TscTimer : public TimerResult {
    using TimerResult::TimerResult;
    long double convert(long double) const override { return -1; }
};

static void run(long double freq, long double s) {
    const std::vector<std::shared_ptr<const TimerResult>> r = {
        std::make_shared<SecondsTimer>("time", "time.h", "seconds", s),
        std::make_shared<ClockTimer>("clock", "time.h", "clock ticks", s),
        std::make_shared<FileTimeTimer>("GetSystemTimeAsFileTime", "windows.h", "100 ns ticks", s),
        std::make_shared<FileTimeTimer>("GetSystemTimePreciseAsFileTime", "windows.h", "100 ns ticks", s),
        std::make_shared<TickCountTimer>("GetTickCount", "windows.h", "milliseconds", s),
        std::make_shared<TscTimer>("__rdtsc", "intrin.h", "TSC ticks/cycles", s),
        std::make_shared<QpcTimer>("QueryPerformanceCounter", "windows.h", "QPC ticks", s, freq),
        std::make_shared<ChronoTimer>("chrono::high_resolution_clock", "chrono", "nanoseconds", s),
        std::make_shared<SecondsTimer>("omp_get_wtime", "omp.h", "seconds", s)};
    for (const auto& e : r)
        std::printf("%s|%s|%s|%.6f|%.9Lf\n", e->name(), e->header(), e->unit(), static_cast<double>(e->minStep()), e->inNanoseconds());
}

// --- тест m2_after ---
void test() {
    long double steps[] = {-1.0L, 0.0L, 1.0L, 15.625L, 0.0001L, 1000.0L};
    for (long double s : steps) run(10000000.0L, s);
    run(3000000.0L, 2.5L);
}

}  // namespace m2_after

namespace m3_before {

struct SavedRow { double raw = 0; double objects = 0; std::string checksum; };
bool loadFile(const std::string& filename, std::map<std::size_t, SavedRow>& rows) {
    auto load = [](const std::string& filename, std::map<std::size_t, SavedRow>& rows) {
            std::ifstream file(filename);
            if (!file) return false;
            std::string line;
            std::getline(file, line); // Skip CSV header
            while (std::getline(file, line)) {
                if (line.empty()) continue;
                std::istringstream in(line);
                std::string nStr, rawStr, objStr, checksumStr;
                if (!std::getline(in, nStr, ',') ||
                    !std::getline(in, rawStr, ',') ||
                    !std::getline(in, objStr, ',') ||
                    !std::getline(in, checksumStr))
                    throw std::runtime_error("Invalid CSV row");
                std::size_t n = std::stoull(nStr);
                rows[n] = { std::stod(rawStr), std::stod(objStr), checksumStr };
            }
            return !rows.empty();
        };
    return load(filename, rows);
}

// --- тест m3_before ---
void run(const char* label, const char* content) {
    { std::ofstream f("in.csv", std::ios::binary); f << content; }
    std::map<std::size_t, SavedRow> rows;
    try {
        bool ok = loadFile("in.csv", rows);
        std::cout << label << " ok=" << ok << " size=" << rows.size() << '\n';
        for (auto& p : rows) std::cout << "  " << p.first << " " << p.second.raw << " " << p.second.objects << " " << p.second.checksum << '\n';
    } catch (const std::exception& e) { std::cout << label << " EXC: " << e.what() << '\n'; }
}

void test() {
    run("normal", "n,raw_seconds,object_seconds,checksum\n512,0.1234,0.2468,262144\n1024,1.5,3.25,524288\n");
    run("dup",    "n,raw_seconds,object_seconds,checksum\n512,0.1,0.2,1\n512,0.3,0.4,2\n");
    run("blank",  "n,raw_seconds,object_seconds,checksum\n\n512,0.1,0.2,1\n\n");
    run("short",  "n,raw_seconds,object_seconds,checksum\n512,0.1\n");
    run("empty",  "n,raw_seconds,object_seconds,checksum\n");
    run("badnum", "n,raw_seconds,object_seconds,checksum\nabc,0.1,0.2,1\n");
    std::map<std::size_t, SavedRow> r; std::cout << "missing=" << loadFile("nofile.csv", r) << '\n';
}

}  // namespace m3_before

namespace m3_after {

class SavedRow {
public:
    SavedRow() = default;
    SavedRow(std::size_t n, double raw, double objects, std::string checksum)
        : n_(n), raw_(raw), objects_(objects), checksum_(std::move(checksum)) {}

    static SavedRow fromCsv(const std::string& line) {
        std::istringstream in(line);
        std::string nStr, rawStr, objStr, checksumStr;
        if (!std::getline(in, nStr, ',') ||
            !std::getline(in, rawStr, ',') ||
            !std::getline(in, objStr, ',') ||
            !std::getline(in, checksumStr))
            throw std::runtime_error("Invalid CSV row");
        return SavedRow(std::stoull(nStr), std::stod(rawStr),
                        std::stod(objStr), checksumStr);
    }

    std::size_t n() const { return n_; }
    double raw() const { return raw_; }
    double objects() const { return objects_; }
    const std::string& checksum() const { return checksum_; }

private:
    std::size_t n_ = 0;
    double raw_ = 0;
    double objects_ = 0;
    std::string checksum_;
};
bool loadFile(const std::string& filename, std::map<std::size_t, SavedRow>& rows) {
    auto load = [](const std::string& filename, std::map<std::size_t, SavedRow>& rows) {
            std::ifstream file(filename);
            if (!file) return false;
            std::string line;
            std::getline(file, line); // Skip CSV header
            while (std::getline(file, line)) {
                if (line.empty()) continue;
                SavedRow row = SavedRow::fromCsv(line);
                rows[row.n()] = row;
            }
            return !rows.empty();
        };
    return load(filename, rows);
}

// --- тест m3_after ---
void run(const char* label, const char* content) {
    { std::ofstream f("in.csv", std::ios::binary); f << content; }
    std::map<std::size_t, SavedRow> rows;
    try {
        bool ok = loadFile("in.csv", rows);
        std::cout << label << " ok=" << ok << " size=" << rows.size() << '\n';
        for (auto& p : rows) std::cout << "  " << p.first << " " << p.second.raw() << " " << p.second.objects() << " " << p.second.checksum() << '\n';
    } catch (const std::exception& e) { std::cout << label << " EXC: " << e.what() << '\n'; }
}

void test() {
    run("normal", "n,raw_seconds,object_seconds,checksum\n512,0.1234,0.2468,262144\n1024,1.5,3.25,524288\n");
    run("dup",    "n,raw_seconds,object_seconds,checksum\n512,0.1,0.2,1\n512,0.3,0.4,2\n");
    run("blank",  "n,raw_seconds,object_seconds,checksum\n\n512,0.1,0.2,1\n\n");
    run("short",  "n,raw_seconds,object_seconds,checksum\n512,0.1\n");
    run("empty",  "n,raw_seconds,object_seconds,checksum\n");
    run("badnum", "n,raw_seconds,object_seconds,checksum\nabc,0.1,0.2,1\n");
    std::map<std::size_t, SavedRow> r; std::cout << "missing=" << loadFile("nofile.csv", r) << '\n';
}

}  // namespace m3_after

int main(int argc, char** argv) {
    const std::string mode = argc > 1 ? argv[1] : "after";
    if (mode == "before") {
        m1_before::test();
        resetFormat();
        m2_before::test();
        resetFormat();
        m3_before::test();
    } else {
        m1_after::test();
        resetFormat();
        m2_after::test();
        resetFormat();
        m3_after::test();
    }
    return 0;
}
