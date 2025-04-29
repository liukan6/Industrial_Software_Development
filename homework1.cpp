#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <stdexcept>
#include <algorithm>
#include <ctime>
#include <windows.h>
#include <vector>
#include <limits>    // 用于清除输入缓冲区

#ifndef LIBRARY_SYSTEM_H
#define LIBRARY_SYSTEM_H

// 类和结构前向声明
class Book;
class Patron;
class Library;
struct Date;
struct Transaction;

// 菜单函数前向声明
void initializeSampleData(Library& lib);
void addBookMenu(Library& lib);
void addPatronMenu(Library& lib);
void checkoutBookMenu(Library& lib);
void returnBookMenu(Library& lib);
void displayAllBooks(const Library& lib);
void displayAllPatrons(const Library& lib);
void displayTransactions(const Library& lib);
void displayDebtors(const Library& lib);

#endif // LIBRARY_SYSTEM_H

// 获取当前年份的独立工具函数
int getCurrentYear() {
    std::time_t t = std::time(nullptr);  // 获取当前时间
    std::tm* now = std::localtime(&t);   // 转换为本地时间结构
    return (now->tm_year + 1900);        // tm_year是从1900开始的年数
}

// 书籍枚举类型
enum class Genre {
    fiction,      // 小说
    nonfiction,   // 非小说类文学作品
    periodical,   // 期刊
    biography,    // 传记
    children      // 儿童读物
};

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
        if (isCheckedOut) {
            throw std::runtime_error("书籍已被借出");
        }
        isCheckedOut = true; 
        std::cout << "【成功】书籍借出成功！\n";  // 添加成功提示
    }
    
    // 归还书籍
    void returnBook() { 
        if (!isCheckedOut) {
            throw std::runtime_error("书籍未被借出");
        }
        isCheckedOut = false;
        std::cout << "【成功】书籍归还成功！\n";  // 添加成功提示
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

    // const std::vector<Book>& getBooks() const { return books; }
    // const std::vector<Patron>& getPatrons() const { return patrons; }
    // const std::vector<Transaction>& getTransactions() const { return transactions; }
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
    const std::vector<Book>& getBooks() const {
        return books;
    }
    
    const std::vector<Patron>& getPatrons() const {
        return patrons;
    }
    
    // 获取所有借阅记录
    const std::vector<Transaction>& getTransactions() const {
        return transactions;
    }
};



int main() {
    // 设置控制台编码为UTF-8以支持中文
    SetConsoleOutputCP(65001);
    
    Library library;
    bool running = true;
    
    // 初始化一些测试数据
    initializeSampleData(library);
    
    while (running) {
        std::cout << "\n====== 图书馆管理系统 ======\n";
        std::cout << "1. 添加书籍\n";
        std::cout << "2. 添加读者\n";
        std::cout << "3. 借出书籍\n";
        std::cout << "4. 归还书籍\n";
        std::cout << "5. 查看所有书籍\n";
        std::cout << "6. 查看所有读者\n";
        std::cout << "7. 查看借阅记录\n";
        std::cout << "8. 查看欠费读者\n";
        std::cout << "0. 退出系统\n";
        std::cout << "请选择操作: ";
        
        int choice;
        std::cin >> choice;
        
        // 清除输入缓冲区
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        try {
            switch (choice) {
                case 1: addBookMenu(library); break;
                case 2: addPatronMenu(library); break;
                case 3: checkoutBookMenu(library); break;
                case 4: returnBookMenu(library); break;
                case 5: displayAllBooks(library); break;
                case 6: displayAllPatrons(library); break;
                case 7: displayTransactions(library); break;
                case 8: displayDebtors(library); break;
                case 0: 
                    running = false;
                    std::cout << "感谢使用图书馆管理系统！\n";
                    break;
                default:
                    std::cout << "无效的选择，请重新输入！\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "操作失败: " << e.what() << "\n";
        }
    }
    
    return 0;
}

// 辅助函数定义
// 初始化示例数据
void initializeSampleData(Library& lib) {
    lib.addBook(Book("111-222-333-A", "1984", "乔治·奥威尔", 1949, Genre::fiction));
    lib.addBook(Book("444-555-666-B", "时间简史", "史蒂芬·霍金", 1988, Genre::nonfiction));
    lib.addBook(Book("777-888-999-C", "国家地理", "多位作者", 2023, Genre::periodical));
    
    Patron p1("张三", 1001);
    Patron p2("李四", 1002);
    Patron p3("王五", 1003);
    p3.setFees(10.50);
    
    lib.addPatron(p1);
    lib.addPatron(p2);
    lib.addPatron(p3);
}

// 添加书籍菜单
void addBookMenu(Library& lib) {
    std::string isbn, title, author;
    int year, genreChoice;
    
    std::cout << "\n=== 添加新书 ===\n";
    
    // 1. 先输入并验证ISBN
    while (true) {
        std::cout << "ISBN(格式n-n-n-x，输入q退出): ";
        std::getline(std::cin, isbn);
        
        if (isbn == "q") {
            std::cout << "已取消添加书籍\n";
            return;
        }
        
        // 创建临时Book对象验证ISBN
        try {
            Book tempBook(isbn, "", "", 0, Genre::fiction); // 会触发ISBN验证
            break; // 如果没抛出异常，说明ISBN有效
        } catch (const std::invalid_argument& e) {
            std::cerr << "错误: " << e.what() << "\n";
            std::cerr << "请重新输入或输入q退出\n";
        }
    }
    
    // 2. ISBN验证通过后，继续输入其他信息
    std::cout << "书名: ";
    std::getline(std::cin, title);
    
    std::cout << "作者: ";
    std::getline(std::cin, author);
    
    // 3. 输入年份（带验证）
    while (true) {
        int currentYear = getCurrentYear();
        std::cout << "出版年份(1-" << currentYear << "，输入q退出): ";
        
        // 先读取为字符串检查是否为空或q
        std::string input;
        std::getline(std::cin, input);
        
        // 允许用户取消输入
        if (input == "q") {
            std::cout << "已取消添加书籍\n";
            return; // 或做其他取消处理
        }
        
        // 尝试转换为数字
        try {
            size_t pos;
            year = std::stoi(input, &pos);
            
            // 检查是否整个字符串都被转换了
            if (pos != input.length()) {
                throw std::invalid_argument("包含非数字字符");
            }
            
            // 验证年份范围
            if (year > 0 && year <= currentYear) {
                break;
            }
            std::cerr << "无效的年份，请输入1-" << currentYear << "之间的数字或输入q取消\n";
        } catch (...) {
            std::cerr << "请输入有效的数字年份(1-" << currentYear << ")或输入q取消\n";
        }
    }
    
    // 4. 输入类型（带验证）
    while (true) {
        std::cout << "选择类型(1.小说 2.非小说 3.期刊 4.传记 5.儿童，输入q退出): ";
        
        std::string input;
        std::getline(std::cin, input);
        
        // 允许用户取消
        if (input == "q") {
            std::cout << "已取消添加书籍\n";
            return;
        }
        
        try {
            size_t pos;
            genreChoice = std::stoi(input, &pos);
            
            // 检查是否整个字符串都是数字
            if (pos != input.length()) {
                throw std::invalid_argument("包含非数字字符");
            }
            
            if (genreChoice >= 1 && genreChoice <= 5) {
                break;
            }
            std::cerr << "无效的选择，请输入1-5之间的数字或输入q退出\n";
        } catch (...) {
            std::cerr << "请输入1-5之间的有效数字或输入q退出\n";
        }
    }
    
    // 5. 创建并添加书籍
    Genre genre = static_cast<Genre>(genreChoice - 1);
    lib.addBook(Book(isbn, title, author, year, genre));
    std::cout << "\n【成功】《" << title << "》已添加到图书馆！\n";
}

// 添加读者菜单
void addPatronMenu(Library& lib) {
    std::string name;
    int cardNumber;
    double fees = 0.0;
    
    std::cout << "\n=== 添加新读者 ===\n";
    
    // 1. 输入姓名
    std::cout << "姓名(输入q退出): ";
    std::getline(std::cin, name);
    if (name == "q") {
        std::cout << "已取消添加读者\n";
        return;
    }

    // 2. 输入借书证号（带验证）
    while (true) {
        std::cout << "借书证号(必须为正整数，输入q退出): ";
        std::string cardInput;
        std::getline(std::cin, cardInput);
        
        if (cardInput == "q") {
            std::cout << "已取消添加读者\n";
            return;
        }
        
        try {
            size_t pos;
            cardNumber = std::stoi(cardInput, &pos);
            
            // 检查是否整个字符串都是数字
            if (pos != cardInput.length()) {
                throw std::invalid_argument("包含非数字字符");
            }
            
            // 检查是否为正值
            if (cardNumber <= 0) {
                throw std::invalid_argument("证号必须为正数");
            }
            
            break; // 输入有效，退出循环
        } catch (...) {
            std::cerr << "错误：请输入有效的正整数借书证号\n";
        }
    }

    // 3. 输入欠费金额（带验证）
    while (true) {
        std::cout << "欠费金额(默认0，输入q退出): ";
        std::string feeInput;
        std::getline(std::cin, feeInput);
        
        if (feeInput == "q") {
            std::cout << "已取消添加读者\n";
            return;
        }
        
        // 允许直接回车使用默认值0
        if (feeInput.empty()) {
            fees = 0.0;
            break;
        }
        
        try {
            size_t pos;
            fees = std::stod(feeInput, &pos);
            
            // 检查是否整个字符串都是有效数字
            if (pos != feeInput.length()) {
                throw std::invalid_argument("包含非法字符");
            }
            
            // 检查是否为非负数
            if (fees < 0) {
                throw std::invalid_argument("欠费金额不能为负数");
            }
            
            break; // 输入有效，退出循环
        } catch (...) {
            std::cerr << "错误：请输入有效的非负数字(如12.5)，或直接回车默认为0\n";
        }
    }

    // 4. 创建并添加读者
    Patron patron(name, cardNumber);
    if (fees > 0) {
        patron.setFees(fees);
    }
    
    lib.addPatron(patron);
    std::cout << "\n【成功】读者 " << name << " (证号:" << cardNumber << ") 已注册！";
    if (fees > 0) {
        std::cout << " 欠费:" << fees << "元";
    }
    std::cout << "\n";
}

// 借书菜单
void checkoutBookMenu(Library& lib) {
    std::string isbn;
    int cardNumber;
    
    std::cout << "\n=== 借出书籍 ===\n";
    std::cout << "输入书籍ISBN: ";
    std::getline(std::cin, isbn);
    
    std::cout << "输入读者借书证号: ";
    std::cin >> cardNumber;
    std::cin.ignore();
    
    // 创建临时对象用于查找
    Book tempBook(isbn, "", "", 0, Genre::fiction);
    Patron tempPatron("", cardNumber);
    
    lib.checkOutBook(tempBook, tempPatron, Date());
    std::cout << "\n【成功】书籍借出成功！\n";
}

// 还书菜单
void returnBookMenu(Library& lib) {
    std::string isbn;
    
    std::cout << "\n=== 归还书籍 ===\n";
    std::cout << "输入要归还的书籍ISBN: ";
    std::getline(std::cin, isbn);
    
    // 在图书馆中查找这本书
    bool found = false;
    for (Book& book : const_cast<std::vector<Book>&>(lib.getBooks())) {
        if (book.getISBN() == isbn) {
            if (!book.getCheckoutStatus()) {
                throw std::runtime_error("这本书没有被借出");
            }
            book.returnBook();
            std::cout << "\n【成功】《" << book.getTitle() << "》已成功归还！\n";
            found = true;
            break;
        }
    }
    
    if (!found) {
        throw std::runtime_error("未找到该ISBN的书籍");
    }
}

// 显示所有书籍
void displayAllBooks(const Library& lib) {
    std::cout << "\n=== 馆藏书籍 ===\n";
    for (const Book& book : lib.getBooks()) {
        std::cout << book << "\n";
        std::cout << "状态: " << (book.getCheckoutStatus() ? "已借出" : "可借") << "\n";
        std::cout << "-----------------\n";
    }
}

// 显示所有读者
void displayAllPatrons(const Library& lib) {
    std::cout << "\n=== 注册读者 ===\n";
    for (const Patron& patron : lib.getPatrons()) {
        std::cout << "姓名: " << patron.getName() << "\n";
        std::cout << "借书证号: " << patron.getCardNumber() << "\n";
        std::cout << "欠费: " << patron.getFees() << "元\n";
        std::cout << "-----------------\n";
    }
}

// 显示借阅记录
void displayTransactions(const Library& lib) {
    const auto& transactions = lib.getTransactions();
    std::cout << "\n=== 借阅记录 ===\n";
    for (const auto& trans : transactions) {
        std::cout << "读者: " << trans.patron.getName() << "\n";
        std::cout << "书籍: " << trans.book.getTitle() << "\n";
        std::cout << "借出日期: " << trans.date.year << "-" 
                  << trans.date.month << "-" << trans.date.day << "\n";
        std::cout << "-----------------\n";
    }
}

// 显示欠费读者
void displayDebtors(const Library& lib) {
    auto debtors = lib.getPatronsWithFees();
    std::cout << "\n=== 欠费读者 ===\n";
    if (debtors.empty()) {
        std::cout << "当前没有欠费读者\n";
    } else {
        for (const auto& name : debtors) {
            std::cout << name << "\n";
        }
    }
}