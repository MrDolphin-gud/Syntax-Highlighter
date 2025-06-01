#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Text_Buffer.H>
#include <string>
#include <vector>
#include <memory>

// Sözdizimi ağacı için düğüm tipleri
// Her düğüm tipi, kodun farklı bir yapısal öğesini temsil eder
enum NodeType {
    PROGRAM,            // Programın kök düğümü
    FUNCTION_DEF,       // Fonksiyon tanımı
    VARIABLE_DECL,      // Değişken tanımı
    PARAM_LIST,         // Parametre listesi
    PARAM,              // Tek bir parametre
    STATEMENT,          // Genel ifade
    IF_STATEMENT,       // If koşul ifadesi
    WHILE_STATEMENT,    // While döngü ifadesi
    FOR_STATEMENT,      // For döngü ifadesi
    RETURN_STATEMENT,   // Return ifadesi
    EXPRESSION,         // Genel ifade
    BINARY_EXPR,        // İkili işlem (+, -, *, /, vb.)
    UNARY_EXPR,         // Tekli işlem
    ASSIGNMENT_EXPR,    // Atama ifadesi
    LITERAL,            // Sabit değer (sayı, metin, karakter)
    IDENTIFIER,         // Tanımlayıcı (değişken, fonksiyon adı)
    TYPE,               // Veri tipi
    BLOCK_STATEMENT     // Kod bloğu
};

// Sözdizimi ağacı düğüm yapısı
// Her düğüm, kodun bir parçasını ve onun alt öğelerini temsil eder
struct ParseNode {
    NodeType type;      // Düğümün tipi
    std::string value;  // Düğümün değeri (operatör, tanımlayıcı, vb.)
    std::vector<std::shared_ptr<ParseNode>> children;  // Alt düğümler
    ParseNode(NodeType t, const std::string& v = "") : type(t), value(v) {}
};

// Yukarıdan aşağıya sözdizimi çözümleyici sınıfı
// Kaynak kodu okuyup sözdizimi ağacını oluşturur
class Parser {
public:
    Fl_Text_Buffer* buffer;    // Kaynak kod metni
    std::string currentToken;  // Şu anki işlenen token
    size_t position;          // Metin içindeki konum

private:
    // Token'ın bir veri tipi olup olmadığını kontrol eder
    bool isType(const std::string& token) {
        static const std::vector<std::string> types = {
            "int", "float", "double", "char", "string", "void", "bool"
        };
        return std::find(types.begin(), types.end(), token) != types.end();
    }

    // Token'ın bir anahtar kelime olup olmadığını kontrol eder
    bool isKeyword(const std::string& token) {
        static const std::vector<std::string> keywords = {
            "if", "else", "while", "for", "return", "break", "continue"
        };
        return std::find(keywords.begin(), keywords.end(), token) != keywords.end();
    }

    // Operatörün önceliğini döndürür
    // Daha yüksek sayı, daha yüksek öncelik anlamına gelir
    int getOperatorPrecedence(const std::string& op) {
        if (op == "*" || op == "/" || op == "%") return 3;  // Çarpma, bölme, mod
        if (op == "+" || op == "-") return 2;               // Toplama, çıkarma
        if (op == "=") return 1;                            // Atama
        return 0;
    }

    // Gereksiz ifade düğümlerini temizler
    // Tek bir alt düğümü olan ifade düğümlerini kaldırır
    std::shared_ptr<ParseNode> cleanExpressionNode(std::shared_ptr<ParseNode> node) {
        if (node->type == EXPRESSION && node->children.size() == 1) {
            return node->children[0];  // Tek alt düğümü olan ifadeyi kaldır
        }
        return node;
    }

    // Yorum satırlarını atlar
    void skipComments() {
        std::string text = buffer->text();
        // Tek satırlık yorum (//)
        if (position + 1 < text.length() && 
            text[position] == '/' && text[position + 1] == '/') {
            position += 2;  // // karakterlerini atla
            // Satır sonuna kadar ilerle
            while (position < text.length() && text[position] != '\n') {
                position++;
            }
            position++;  // \n karakterini atla
        }
        // Çok satırlı yorum (/* */)
        else if (position + 1 < text.length() && 
                 text[position] == '/' && text[position + 1] == '*') {
            position += 2;  // /* karakterlerini atla
            // */ karakterlerini bulana kadar ilerle
            while (position + 1 < text.length() && 
                   !(text[position] == '*' && text[position + 1] == '/')) {
                position++;
            }
            position += 2;  // */ karakterlerini atla
        }
    }

    // Bir sonraki token'ı alır
    // Boşlukları ve yorumları atlar ve bir sonraki anlamlı token'ı döndürür
    std::string getNextToken() {
        std::string text = buffer->text();
        while (position < text.length()) {
            // Boşlukları atla
            if (isspace(text[position])) {
                position++;
                continue;
            }
            // Yorumları atla
            if (position + 1 < text.length() && 
                text[position] == '/' && 
                (text[position + 1] == '/' || text[position + 1] == '*')) {
                skipComments();
                continue;
            }
            break;  // Anlamlı bir karakter bulundu
        }
        if (position >= text.length()) {
            return "";
        }
        size_t start = position;
        // Tanımlayıcı (değişken/fonksiyon adı)
        if (isalpha(text[position]) || text[position] == '_') {
            while (position < text.length() && (isalnum(text[position]) || text[position] == '_')) {
                position++;
            }
        }
        // Sayı
        else if (isdigit(text[position])) {
            while (position < text.length() && (isdigit(text[position]) || text[position] == '.')) {
                position++;
            }
        }
        // Metin (çift tırnak içinde)
        else if (text[position] == '"') {
            position++;
            while (position < text.length() && text[position] != '"') {
                if (text[position] == '\\') position++;  // Kaçış karakterlerini atla
                position++;
            }
            position++;
        }
        // Karakter (tek tırnak içinde)
        else if (text[position] == '\'') {
            position++;
            while (position < text.length() && text[position] != '\'') {
                if (text[position] == '\\') position++;  // Kaçış karakterlerini atla
                position++;
            }
            position++;
        }
        // Operatör veya diğer karakterler
        else {
            position++;
        }
        return text.substr(start, position - start);
    }

    // Temel ifadeleri ayrıştırır (tanımlayıcılar, sabitler, parantezli ifadeler)
    std::shared_ptr<ParseNode> parsePrimary() {
        // Parantezli ifade
        if (currentToken == "(") {
            currentToken = getNextToken();
            auto expr = cleanExpressionNode(parseExpression());
            if (currentToken == ")") {
                currentToken = getNextToken();
            }
            return expr;
        }
        // Tanımlayıcı (değişken/fonksiyon adı)
        if (isalpha(currentToken[0]) || currentToken[0] == '_') {
            auto node = std::make_shared<ParseNode>(IDENTIFIER, currentToken);
            currentToken = getNextToken();
            return node;
        }
        // Sabit değer (sayı, metin, karakter)
        else if (isdigit(currentToken[0]) || currentToken[0] == '"' || currentToken[0] == '\'') {
            auto node = std::make_shared<ParseNode>(LITERAL, currentToken);
            currentToken = getNextToken();
            return node;
        }
        // Hata durumu
        currentToken = getNextToken();
        return std::make_shared<ParseNode>(EXPRESSION);
    }

    // İfadeleri operatör önceliğine göre ayrıştırır
    std::shared_ptr<ParseNode> parseExpression(int minPrecedence = 0) {
        auto left = cleanExpressionNode(parsePrimary());
        // Operatörleri işle
        while (!currentToken.empty() && 
               (currentToken == "+" || currentToken == "-" || 
                currentToken == "*" || currentToken == "/" || 
                currentToken == "%" || currentToken == "=")) {
            std::string op = currentToken;
            int precedence = getOperatorPrecedence(op);
            // Operatör önceliği kontrolü
            if (precedence < minPrecedence) {
                break;
            }
            currentToken = getNextToken();
            auto right = cleanExpressionNode(parseExpression(precedence + 1));
            // İkili işlem düğümü oluştur
            if (!op.empty()) {
                auto binaryNode = std::make_shared<ParseNode>(BINARY_EXPR);
                binaryNode->value = op;
                binaryNode->children.push_back(left);
                binaryNode->children.push_back(right);
                left = binaryNode;
            }
        }
        return left;
    }
    // Atama ifadelerini ayrıştırır
    std::shared_ptr<ParseNode> parseAssignment() {
        auto left = parsePrimary();
        if (currentToken == "=") {
            auto assignNode = std::make_shared<ParseNode>(ASSIGNMENT_EXPR);
            assignNode->children.push_back(left);
            currentToken = getNextToken();
            assignNode->children.push_back(parseExpression());
            return assignNode;
        }
        return left;
    }

    // Değişken tanımlamalarını ayrıştırır
    std::shared_ptr<ParseNode> parseVariableDecl() {
        auto node = std::make_shared<ParseNode>(VARIABLE_DECL);
        // Veri tipini ayrıştır
        node->children.push_back(std::make_shared<ParseNode>(TYPE, currentToken));
        currentToken = getNextToken();
        // Değişken adını ayrıştır
        node->children.push_back(std::make_shared<ParseNode>(IDENTIFIER, currentToken));
        currentToken = getNextToken();
        // İlk değer ataması varsa ayrıştır
        if (currentToken == "=") {
            currentToken = getNextToken();
            node->children.push_back(parseExpression());
        }
        // Noktalı virgülü atla
        if (currentToken == ";") {
            currentToken = getNextToken();
        }
        return node;
    }

    // Parametreleri ayrıştırır
    std::shared_ptr<ParseNode> parseParameter() {
        auto param = std::make_shared<ParseNode>(PARAM);
        // Parametre tipini ayrıştır
        if (isType(currentToken)) {
            param->children.push_back(std::make_shared<ParseNode>(TYPE, currentToken));
            currentToken = getNextToken();
            // Parametre adını ayrıştır
            if (!isKeyword(currentToken)) {
                param->children.push_back(std::make_shared<ParseNode>(IDENTIFIER, currentToken));
                currentToken = getNextToken();
            }
        }
        return param;
    }

    // Kod bloklarını ayrıştırır
    std::shared_ptr<ParseNode> parseBlock() {
        auto block = std::make_shared<ParseNode>(BLOCK_STATEMENT);
        while (currentToken != "}" && !currentToken.empty()) {
            // Noktalı virgülleri atla
            if (currentToken == ";") {
                currentToken = getNextToken();
                continue;
            }
            // İfadeleri ayrıştır
            auto stmt = parseStatement();
            if (stmt->type != EXPRESSION || !stmt->children.empty()) {
                block->children.push_back(stmt);
            }
        }
        return block;
    }

    // İfadeleri ayrıştırır (if, while, for, return, değişken tanımı, atama)
    std::shared_ptr<ParseNode> parseStatement() {
        if (currentToken == "if") {
            return parseIfStatement();
        } else if (currentToken == "while") {
            return parseWhileStatement();
        } else if (currentToken == "for") {
            return parseForStatement();
        } else if (currentToken == "return") {
            return parseReturnStatement();
        } else if (isType(currentToken)) {
            return parseVariableDecl();
        } else {
            auto stmt = parseAssignment();
            if (currentToken == ";") {
                currentToken = getNextToken();
            }
            return stmt;
        }
    }

    // If ifadelerini ayrıştırır
    std::shared_ptr<ParseNode> parseIfStatement() {
        auto node = std::make_shared<ParseNode>(IF_STATEMENT);
        currentToken = getNextToken(); // if'i atla
        // Koşul ifadesini ayrıştır
        if (currentToken == "(") {
            currentToken = getNextToken();
            node->children.push_back(parseExpression());
            if (currentToken == ")") {
                currentToken = getNextToken();
            }
        }
        // If bloğunu ayrıştır
        if (currentToken == "{") {
            currentToken = getNextToken();
            node->children.push_back(parseBlock());
            if (currentToken == "}") {
                currentToken = getNextToken();
            }
        }
        // Else bloğunu ayrıştır
        if (currentToken == "else") {
            currentToken = getNextToken();
            if (currentToken == "{") {
                currentToken = getNextToken();
                node->children.push_back(parseBlock());
                if (currentToken == "}") {
                    currentToken = getNextToken();
                }
            }
        }
        return node;
    }
    // While döngülerini ayrıştırır
    std::shared_ptr<ParseNode> parseWhileStatement() {
        auto node = std::make_shared<ParseNode>(WHILE_STATEMENT);
        currentToken = getNextToken(); // while'i atla
        // Koşul ifadesini ayrıştır
        if (currentToken == "(") {
            currentToken = getNextToken();
            node->children.push_back(parseExpression());
            if (currentToken == ")") {
                currentToken = getNextToken();
            }
        }
        // While bloğunu ayrıştır
        if (currentToken == "{") {
            currentToken = getNextToken();
            node->children.push_back(parseBlock());
            if (currentToken == "}") {
                currentToken = getNextToken();
            }
        }
        return node;
    }
    // For döngülerini ayrıştırır
    std::shared_ptr<ParseNode> parseForStatement() {
        auto node = std::make_shared<ParseNode>(FOR_STATEMENT);
        currentToken = getNextToken(); // for'u atla
        if (currentToken == "(") {
            currentToken = getNextToken();
            // Başlangıç ifadesi
            node->children.push_back(parseExpression());
            if (currentToken == ";") {
                currentToken = getNextToken();
            }
            // Koşul ifadesi
            node->children.push_back(parseExpression());
            if (currentToken == ";") {
                currentToken = getNextToken();
            }
            // Artırma ifadesi
            node->children.push_back(parseExpression());
            if (currentToken == ")") {
                currentToken = getNextToken();
            }
        }
        // For bloğunu ayrıştır
        if (currentToken == "{") {
            currentToken = getNextToken();
            node->children.push_back(parseBlock());
            if (currentToken == "}") {
                currentToken = getNextToken();
            }
        }
        return node;
    }
    // Return ifadelerini ayrıştırır
    std::shared_ptr<ParseNode> parseReturnStatement() {
        auto node = std::make_shared<ParseNode>(RETURN_STATEMENT);
        currentToken = getNextToken(); // return'u atla
        // Dönüş değerini ayrıştır
        node->children.push_back(parseExpression());
        if (currentToken == ";") {
            currentToken = getNextToken();
        }
        return node;
    }
    // Fonksiyon tanımlarını ayrıştırır
    std::shared_ptr<ParseNode> parseFunctionDef() {
        auto node = std::make_shared<ParseNode>(FUNCTION_DEF);
        // Dönüş tipini ayrıştır
        if (isType(currentToken)) {
            node->children.push_back(std::make_shared<ParseNode>(TYPE, currentToken));
            currentToken = getNextToken();
        }
        // Fonksiyon adını ayrıştır
        if (!isKeyword(currentToken)) {
            node->children.push_back(std::make_shared<ParseNode>(IDENTIFIER, currentToken));
            currentToken = getNextToken();
        }
        // Parametre listesini ayrıştır
        auto paramList = std::make_shared<ParseNode>(PARAM_LIST);
        node->children.push_back(paramList);
        if (currentToken == "(") {
            currentToken = getNextToken();
            // Parametreleri ayrıştır
            while (currentToken != ")" && !currentToken.empty()) {
                if (isType(currentToken)) {
                    paramList->children.push_back(parseParameter());
                    if (currentToken == ",") {
                        currentToken = getNextToken();
                    }
                } else {
                    currentToken = getNextToken();
                }
            }
            currentToken = getNextToken(); // )'yi atla
        }
        // Fonksiyon gövdesini ayrıştır
        if (currentToken == "{") {
            currentToken = getNextToken();
            node->children.push_back(parseBlock());
            if (currentToken == "}") {
                currentToken = getNextToken();
            }
        }
        return node;
    }

public:
    // Yapıcı fonksiyon
    Parser(Fl_Text_Buffer* buf) : buffer(buf), position(0) {}
    // Kaynak kodu ayrıştırır ve sözdizimi ağacını oluşturur
    std::shared_ptr<ParseNode> parse() {
        auto root = std::make_shared<ParseNode>(PROGRAM);
        currentToken = getNextToken();
        while (!currentToken.empty()) {
            // Fonksiyon tanımı veya değişken tanımı
            if (isType(currentToken)) {
                std::string nextToken = getNextToken();
                position -= nextToken.length();
                if (nextToken == "(") {
                    root->children.push_back(parseFunctionDef());
                } else {
                    root->children.push_back(parseVariableDecl());
                }
            }
            // Diğer ifadeler
            else if (!isKeyword(currentToken) && currentToken != ";") {
                auto stmt = parseStatement();
                if (stmt->type != EXPRESSION || !stmt->children.empty()) {
                    root->children.push_back(stmt);
                }
            } else {
                currentToken = getNextToken();
            }
        }
        return root;
    }
};

// Sözdizimi ağacı görselleştirme penceresi
class ParseTreeWindow : public Fl_Window {
private:
    Fl_Tree* tree;              // Ağaç görünümü
    Fl_Text_Buffer* buffer;     // Kaynak kod metni
    bool isVisible;             // Pencere görünürlüğü
    Parser* parser;             // Sözdizimi çözümleyici
    static const char* getNodeTypeName(NodeType type) {
        switch (type) {
            case PROGRAM: return "Program";
            case FUNCTION_DEF: return "Fonksiyon Tanımı";
            case VARIABLE_DECL: return "Değişken Tanımı";
            case PARAM_LIST: return "Parametre Listesi";
            case PARAM: return "Parametre";
            case STATEMENT: return "İfade";
            case IF_STATEMENT: return "If İfadesi";
            case WHILE_STATEMENT: return "While Döngüsü";
            case FOR_STATEMENT: return "For Döngüsü";
            case RETURN_STATEMENT: return "Return İfadesi";
            case EXPRESSION: return "İfade";
            case BINARY_EXPR: return "İkili İşlem";
            case UNARY_EXPR: return "Tekli İşlem";
            case ASSIGNMENT_EXPR: return "Atama İfadesi";
            case LITERAL: return "Sabit Değer";
            case IDENTIFIER: return "Tanımlayıcı";
            case TYPE: return "Veri Tipi";
            case BLOCK_STATEMENT: return "Kod Bloğu";
            default: return "Bilinmeyen";
        }
    }
    // Düğümü ağaca ekler
    void addNodeToTree(Fl_Tree_Item* parent, const std::shared_ptr<ParseNode>& node) {
        std::string label = getNodeTypeName(node->type);
        if (!node->value.empty()) {
            label += ": " + node->value;
        }
        Fl_Tree_Item* item = tree->add(parent, label.c_str());
        for (const auto& child : node->children) {
            addNodeToTree(item, child);
        }
    }

public:
    // Yapıcı fonksiyon
    ParseTreeWindow(int w, int h, const char* title, Fl_Text_Buffer* buf)
        : Fl_Window(w, h, "Sözdizimi Ağacı"), buffer(buf), isVisible(false) {
        tree = new Fl_Tree(10, 10, w-20, h-20);
        tree->showroot(0);                    // Kök düğümü gizle
        tree->connectorstyle(FL_TREE_CONNECTOR_DOTTED);  // Noktalı bağlantı çizgileri
        tree->selectmode(FL_TREE_SELECT_NONE);  // Seçimi devre dışı bırak
        parser = new Parser(buffer);          // Çözümleyiciyi oluştur
        end();
    }
    // Yıkıcı fonksiyon
    ~ParseTreeWindow() {
        delete parser;  // Çözümleyiciyi temizle
    }
    // Ağacı günceller
    void updateTree() {
        if (!isVisible) return;
        tree->clear();
        parser->position = 0;  // Çözümleyici konumunu sıfırla
        auto root = parser->parse();
        Fl_Tree_Item* rootItem = tree->add("Program");
        rootItem->open();
        for (const auto& child : root->children) {
            addNodeToTree(rootItem, child);
        }
        tree->redraw();
    }
    // Pencereyi göster
    void show() {
        isVisible = true;
        Fl_Window::show();
        updateTree();
    }
    // Pencereyi gizle
    void hide() {
        isVisible = false;
        Fl_Window::hide();
    }
};

// Metin değiştiğinde çağrılan callback
void metin_degisti_parse_cb(int pos, int nInserted, int nDeleted, int, const char*, void* v) {
    if (nInserted || nDeleted) {
        ParseTreeWindow* window = static_cast<ParseTreeWindow*>(v);
        window->updateTree();
    }
}

// Sözdizimi ağacını gösteren callback
void parse_tree_cb(Fl_Widget*, void* v) {
    ParseTreeWindow* window = static_cast<ParseTreeWindow*>(v);
    window->show();
} 
