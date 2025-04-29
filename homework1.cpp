#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <stdexcept>
#include <algorithm>
#include <ctime>
#include <windows.h>

// 书籍枚举类型
enum class Genre {
    fiction,      // 小说
    nonfiction,   // 非小说类文学作品
    periodical,   // 期刊
    biography,    // 传记
    children      // 儿童读物
};

// 前向声明
class Book;
class Patron;

// 日期结构体，用于记录借阅日期
struct Date {
    int year;    // 年
    int month;   // 月
    int day;     // 日

    // 默认构造函数，获取当前日期
    Date() {
        time_t now = time(0);       // 获取当前时间
        tm *ltm = localtime(&now);  // 转换为本地时间结构
        year = 1900 + ltm->tm_year; // 年份(从1900开始)
        month = 1 + ltm->tm_mon;    // 月份(0-11)
        day = ltm->tm_mday;         // 日
    }

    // 带参数的构造函数，指定日期
    Date(int y, int m, int d) : year(y), month(m), day(d) {}
};

// 图书类
class Book {
private:
    std::string isbn;       // ISBN号
    std::string title;      // 书名
    std::string author;     // 作者
    int copyrightYear;      // 版权年份
    Genre genre;            // 书籍类型
    bool isCheckedOut;      // 是否借出

    // ISBN验证函数(n-n-n-x格式)
    bool isValidISBN(const std::string& isbn) {
        std::regex pattern("^\\d+-\\d+-\\d+-[a-zA-Z0-9]$");
        return std::regex_match(isbn, pattern);
    }

public:
    // 构造函数
    Book(const std::string& i, const std::string& t, const std::string& a, int year, Genre g)
        : title(t), author(a), copyrightYear(year), genre(g), isCheckedOut(false) {
        if (!isValidISBN(i)) {
            throw std::invalid_argument("无效的ISBN格式，应为n-n-n-x");
        }
        isbn = i;
    }

    // 获取ISBN号
    std::string getISBN() const { return isbn; }
    // 获取书名
    std::string getTitle() const { return title; }
    // 获取作者
    std::string getAuthor() const { return author; }
    // 获取版权年份
    int getCopyrightYear() const { return copyrightYear; }
    // 获取书籍类型
    Genre getGenre() const { return genre; }
    // 获取借阅状态
    bool getCheckoutStatus() const { return isCheckedOut; }

    // 借出书籍
    void checkOut() { 
        if (isCheckedOut) throw std::runtime_error("书籍已被借出");
        isCheckedOut = true; 
    }
    
    // 归还书籍
    void returnBook() { 
        if (!isCheckedOut) throw std::runtime_error("书籍未被借出");
        isCheckedOut = false; 
    }

    // 重载==运算符，比较ISBN号
    bool operator==(const Book& other) const { return isbn == other.isbn; }
    // 重载!=运算符
    bool operator!=(const Book& other) const { return !(*this == other); }

    // 友元函数，重载输出运算符
    friend std::ostream& operator<<(std::ostream& os, const Book& book);
};

// 重载<<运算符，输出书籍信息
std::ostream& operator<<(std::ostream& os, const Book& book) {
    os << "书名: " << book.title << "\n"
       << "作者: " << book.author << "\n"
       << "ISBN: " << book.isbn << "\n"
       << "类型: ";
    // 根据枚举值输出类型名称
    switch(book.genre) {
        case Genre::fiction: os << "小说"; break;
        case Genre::nonfiction: os << "非小说类文学作品"; break;
        case Genre::periodical: os << "期刊"; break;
        case Genre::biography: os << "传记"; break;
        case Genre::children: os << "儿童读物"; break;
    }
    return os;
}

// 读者类
class Patron {
private:
    std::string name;       // 读者姓名
    int cardNumber;         // 借书证号
    double fees;            // 欠费金额

public:
    // 构造函数
    Patron(const std::string& n, int cn) : name(n), cardNumber(cn), fees(0) {}

    // 获取读者姓名
    std::string getName() const { return name; }
    // 获取借书证号
    int getCardNumber() const { return cardNumber; }
    // 获取欠费金额
    double getFees() const { return fees; }

    // 检查是否有欠费
    bool owesFees() const { return fees > 0; }

    // 设置欠费金额
    void setFees(double f) {
        if (f < 0) throw std::invalid_argument("欠费金额不能为负数");
        fees = f;
    }
};

// 借阅记录结构体
struct Transaction {
    Book book;      // 借阅的书籍
    Patron patron;  // 借阅的读者
    Date date;      // 借阅日期
};

// 图书馆类
class Library {
private:
    std::vector<Book> books;                // 馆藏书籍集合
    std::vector<Patron> patrons;            // 注册读者集合
    std::vector<Transaction> transactions;  // 借阅记录集合

public:
    // 添加书籍到图书馆
    void addBook(const Book& book) {
        books.push_back(book);
    }

    // 添加读者到图书馆
    void addPatron(const Patron& patron) {
        patrons.push_back(patron);
    }

    // 借出书籍
    void checkOutBook(const Book& book, const Patron& patron, const Date& date) {
        // 检查书籍是否在馆藏中
        auto bookIt = std::find(books.begin(), books.end(), book);
        if (bookIt == books.end()) {
            throw std::runtime_error("图书馆中没有这本书");
        }

        // 检查读者是否注册
        auto patronIt = std::find_if(patrons.begin(), patrons.end(),
            [&patron](const Patron& p) { return p.getCardNumber() == patron.getCardNumber(); });
        if (patronIt == patrons.end()) {
            throw std::runtime_error("读者未注册");
        }

        // 检查读者是否有欠费
        if (patronIt->owesFees()) {
            throw std::runtime_error("读者有欠费，不能借书");
        }

        // 检查书籍是否可借
        if (bookIt->getCheckoutStatus()) {
            throw std::runtime_error("书籍已被借出");
        }

        // 创建借阅记录
        transactions.push_back(Transaction{*bookIt, *patronIt, date});

        // 更新书籍状态为已借出
        const_cast<Book&>(*bookIt).checkOut();
    }

    // 获取所有欠费读者名单
    std::vector<std::string> getPatronsWithFees() const {
        std::vector<std::string> result;
        for (const auto& patron : patrons) {
            if (patron.owesFees()) {
                result.push_back(patron.getName());
            }
        }
        return result;
    }

    // 获取所有借阅记录
    const std::vector<Transaction>& getTransactions() const {
        return transactions;
    }
};

// 示例使用
int main() {
    // 设置控制台编码为UTF-8
    SetConsoleOutputCP(65001);
    try {
        // 创建图书馆对象
        Library library;

        // 添加不同种类的书籍
        library.addBook(Book("111-222-333-A", "1984", "乔治·奥威尔", 1949, Genre::fiction));
        library.addBook(Book("444-555-666-B", "时间简史", "史蒂芬·霍金", 1988, Genre::nonfiction));
        library.addBook(Book("777-888-999-C", "国家地理", "多位作者", 2023, Genre::periodical));

        // 添加读者
        library.addPatron(Patron("张三", 1001));
        library.addPatron(Patron("李四", 1002));
        Patron badPatron("王五", 1003);
        badPatron.setFees(10.50); // 这个读者有欠费
        library.addPatron(badPatron);

        // 借出书籍
        Book bookToCheckOut = Book("111-222-333-A", "", "", 0, Genre::fiction); // 用于查找的临时对象
        Patron patron = Patron("张三", 1001); // 用于查找的临时对象
        library.checkOutBook(bookToCheckOut, patron, Date());

        // 尝试用有欠费的读者借书(应该失败)
        try {
            library.checkOutBook(Book("444-555-666-B", "", "", 0, Genre::nonfiction), 
                            badPatron, Date());
        } catch (const std::exception& e) {
            std::cout << "预期错误: " << e.what() << "\n";
        }

        // 获取欠费读者名单
        auto debtors = library.getPatronsWithFees();
        std::cout << "\n欠费读者:\n";
        for (const auto& name : debtors) {
            std::cout << name << "\n";
        }

        // 显示借阅记录
        std::cout << "\n借阅记录:\n";
        for (const auto& trans : library.getTransactions()) {
            std::cout << "读者: " << trans.patron.getName() 
                      << ", 书籍: " << trans.book.getTitle() 
                      << ", 日期: " << trans.date.year << "-" 
                      << trans.date.month << "-" << trans.date.day << "\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << "\n";
    }

    return 0;
}