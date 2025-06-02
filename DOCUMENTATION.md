# C++ Syntax Vurgulayıcı ve Sözdizimi Ağacı Dokümantasyonu

## İçindekiler
1. [Genel Bakış](#genel-bakış)
2. [Kurulum ve Gereksinimler](#kurulum-ve-gereksinimler)
3. [Syntax Vurgulayıcı](#syntax-vurgulayıcı)
   - [Token Tipleri](#token-tipleri)
   - [SyntaxVurgulayici Sınıfı](#syntaxvurgulayici-sınıfı)
   - [Token İşleme](#token-işleme)
   - [Regex Desenleri](#regex-desenleri)
   - [Renk ve Stil Yönetimi](#renk-ve-stil-yönetimi)
4. [Sözdizimi Ağacı](#sözdizimi-ağacı)
   - [Düğüm Tipleri](#düğüm-tipleri)
   - [Parser Sınıfı](#parser-sınıfı)
   - [Ağaç Yapısı](#ağaç-yapısı)
   - [Ayrıştırma Stratejisi](#ayrıştırma-stratejisi)
   - [Hata Yönetimi](#hata-yönetimi)
5. [Kullanıcı Arayüzü](#kullanıcı-arayüzü)
   - [Ana Pencere](#ana-pencere)
   - [Lexical Analiz Penceresi](#lexical-analiz-penceresi)
   - [Sözdizimi Ağacı Penceresi](#sözdizimi-ağacı-penceresi)
6. [Örnek Kullanım](#örnek-kullanım)
   - [Basit Örnekler](#basit-örnekler)
   - [Karmaşık Örnekler](#karmaşık-örnekler)


## Genel Bakış

Bu proje, C++ kaynak kodlarını analiz eden ve görselleştiren iki ana bileşenden oluşmaktadır:

1. **Syntax Vurgulayıcı**: Kaynak kodundaki farklı token'ları (anahtar kelimeler, tanımlayıcılar, sayılar vb.) renklendirerek görsel olarak ayırt edilmesini sağlar.
2. **Sözdizimi Ağacı**: Kaynak kodun yapısal analizini yaparak, kodun hiyerarşik bir ağaç yapısında gösterilmesini sağlar.

Proje FLTK (Fast Light Toolkit) kütüphanesi kullanılarak geliştirilmiştir ve modern bir grafiksel kullanıcı arayüzü sunmaktadır.

### Temel Özellikler

- Gerçek zamanlı syntax vurgulama
- Detaylı lexical analiz görüntüleme
- Hiyerarşik sözdizimi ağacı gösterimi
- Türkçe karakter desteği
- Modern ve kullanıcı dostu arayüz
- Yüksek performanslı kod analizi
- Bellek içi işlem (dosya sistemi kullanılmaz)

## Kurulum ve Gereksinimler

### Sistem Gereksinimleri

- C++17 veya üzeri derleyici
- FLTK 1.4.x kütüphanesi
- CMake 3.10 veya üzeri
- Modern bir işletim sistemi (Windows, Linux, macOS)

### Derleme Adımları

1. FLTK kütüphanesini yükleyin:
```bash
# Ubuntu/Debian
sudo apt-get install libfltk1.3-dev

# Windows (vcpkg)
vcpkg install fltk:x64-windows
```

2. Projeyi derleyin:
```bash
mkdir build
cd build
cmake ..
make
```

### Bağımlılıklar

Proje aşağıdaki bağımlılıkları kullanır:

```cpp
#include <FL/Fl.H>              // FLTK ana başlık
#include <FL/Fl_Window.H>       // Pencere sınıfı
#include <FL/Fl_Text_Editor.H>  // Metin düzenleyici
#include <FL/Fl_Text_Buffer.H>  // Metin tamponu
#include <FL/Fl_Tree.H>         // Ağaç görünümü
#include <regex>                // Regex işlemleri
#include <vector>               // Dinamik diziler
#include <string>               // String işlemleri
#include <memory>               // Akıllı işaretçiler
```

## Syntax Vurgulayıcı

### Token Tipleri

Program, kaynak kodundaki farklı öğeleri aşağıdaki token tipleriyle sınıflandırır:

```cpp
enum TokenTipi {
    ANAHTAR_KELIME = 0,  // if, else, while gibi C++ anahtar kelimeleri
    TANIMLAYICI,         // Değişken ve fonksiyon isimleri
    SAYI,                // Sayısal değerler (tam sayı ve ondalıklı)
    OPERATOR,            // +, -, *, /, =, ==, != gibi operatörler
    METIN,               // Çift tırnak içindeki string'ler
    KARAKTER,            // Tek tırnak içindeki karakterler
    YORUM,               // // veya /* */ içindeki yorumlar
    BOSLUK,              // Boşluk, tab ve satır sonları
};
```

### Token Yapısı

Her token, aşağıdaki bilgileri içerir:

```cpp
struct Token {
    int baslangic;       // Token'ın metin içindeki başlangıç pozisyonu
    int bitis;           // Token'ın metin içindeki bitiş pozisyonu
    TokenTipi tip;       // Token'ın tipi
    std::string deger;   // Token'ın değeri (opsiyonel)
};
```

### SyntaxVurgulayici Sınıfı

`SyntaxVurgulayici` sınıfı, kaynak kodun token'lara ayrılması ve renklendirilmesi işlemlerini yönetir:

```cpp
class SyntaxVurgulayici {
private:
    Fl_Text_Buffer *metinBuffer;           // Düzenlenen metni tutan buffer
    Fl_Text_Buffer *stilBuffer;            // Renk ve stil bilgilerini tutan buffer
    std::vector<Token> tokenlar;           // Bulunan tüm token'ların listesi
    std::map<TokenTipi, Fl_Color> renkHaritasi;  // Token tiplerine göre renk eşleştirmeleri
    std::vector<std::string> anahtarKelimeler;   // C++ anahtar kelimeleri listesi
    std::regex anahtarKelimeRegex;        // Anahtar kelimeleri bulmak için
    std::regex tanimlayiciRegex;          // Tanımlayıcıları bulmak için
    std::regex sayiRegex;                 // Sayıları bulmak için
    std::regex operatorRegex;             // Operatörleri bulmak için
    std::regex metinRegex;                // String'leri bulmak için
    std::regex karakterRegex;             // Karakterleri bulmak için
    std::regex yorumRegex;                // Yorumları bulmak için

public:
    // Constructor
    SyntaxVurgulayici(Fl_Text_Buffer *buf);
    
    // Token işleme metodları
    void tokenize();
    void vurgula();
    
    // Yardımcı metodlar
    std::string anahtarKelimeDeseniAl();
    bool isKeyword(const std::string& token);
    bool isType(const std::string& token);
    
    // Getter metodları
    Fl_Text_Buffer* getStilBuffer();
    const std::vector<Token>& getTokenlar() const;
};
```

### Regex Desenleri

Program, farklı token tiplerini bulmak için aşağıdaki regex desenlerini kullanır:

```cpp
// Anahtar kelimeler için desen
std::string anahtarKelimeDeseni = "\\b(if|else|while|for|return|int|float|void|char|string)\\b";

// Tanımlayıcılar için desen (değişken/fonksiyon isimleri)
std::string tanimlayiciDeseni = "[a-zA-Z_]\\w*";

// Sayılar için desen (tam sayı ve ondalıklı)
std::string sayiDeseni = "\\b\\d+(\\.\\d+)?\\b";

// Operatörler için desen
std::string operatorDeseni = "[\\+\\-\\*/%=&|<>!]";

// String'ler için desen
std::string metinDeseni = "\"(?:[^\"\\\\]|\\\\.)*\"";

// Karakterler için desen
std::string karakterDeseni = "'(?:[^'\\\\]|\\\\.)'";

// Yorumlar için desen
std::string yorumDeseni = "(//[^\\n]*)|(/\\*[\\s\\S]*?\\*/)";
```

### Token İşleme

Token'ların işlenmesi şu adımlarla gerçekleşir:

1. **Token Bulma**: Regex desenleri kullanılarak metin içindeki token'lar bulunur:
```cpp
void tokenize() {
    tokenlar.clear();
    std::string metin = metinBuffer->text();
    if (metin.empty()) return;

    // String'leri bul
    auto string_baslangic = std::sregex_iterator(metin.begin(), metin.end(), metinRegex);
    auto string_bitis = std::sregex_iterator();
    for (std::sregex_iterator i = string_baslangic; i != string_bitis; ++i) {
        std::smatch eslesme = *i;
        Token token;
        token.baslangic = eslesme.position();
        token.bitis = token.baslangic + eslesme.length();
        token.tip = METIN;
        tokenlar.push_back(token);
    }

    // Diğer token'ları bul...
    // Anahtar kelimeler
    auto kelime_baslangic = std::sregex_iterator(metin.begin(), metin.end(), anahtarKelimeRegex);
    // Sayılar
    auto sayi_baslangic = std::sregex_iterator(metin.begin(), metin.end(), sayiRegex);
    // Operatörler
    auto operator_baslangic = std::sregex_iterator(metin.begin(), metin.end(), operatorRegex);
    // Tanımlayıcılar
    auto tanimlayici_baslangic = std::sregex_iterator(metin.begin(), metin.end(), tanimlayiciRegex);

    // Token'ları sırala
    std::sort(tokenlar.begin(), tokenlar.end(),
             [](const Token &a, const Token &b) { return a.baslangic < b.baslangic; });
}
```

2. **Renklendirme**: Bulunan token'lar renklendirilir:
```cpp
void vurgula() {
    if (metinBuffer->length() == 0) return;

    // Önce tüm metni tanımlayıcı rengiyle doldur
    std::string stil(metinBuffer->length(), 'A' + TANIMLAYICI);
    stilBuffer->text(stil.c_str());

    // Her token için uygun rengi uygula
    for (const Token &token : tokenlar) {
        if (token.tip == BOSLUK) continue;
        char stilKarakteri = 'A' + token.tip;
        for (int i = token.baslangic; i < token.bitis; ++i) {
            stilBuffer->replace(i, i + 1, &stilKarakteri, 1);
        }
    }
}
```

### Renk ve Stil Yönetimi

Program, farklı token tipleri için özel renk ve stil tanımlamaları kullanır:

```cpp
// Stil tablosu tanımlaması
Fl_Text_Display::Style_Table_Entry stilTablosu[] = {
    { FL_RED,        FL_COURIER_BOLD, 14 },    // A - ANAHTAR_KELIME
    { FL_BLACK,      FL_COURIER, 14 },         // B - TANIMLAYICI
    { FL_BLUE,       FL_COURIER, 14 },         // C - SAYI
    { FL_DARK_GREEN, FL_COURIER, 14 },         // D - OPERATOR
    { FL_MAGENTA,    FL_COURIER, 14 },         // E - METIN
    { FL_DARKGOLD,   FL_COURIER, 14 },         // F - KARAKTER
    { FL_GRAY,       FL_COURIER_ITALIC, 14 },  // G - YORUM
    { FL_WHITE,      FL_COURIER, 14 }          // H - BOSLUK
};

// Stil tablosunu düzenleyiciye uygula
duzenleyici->highlight_data(vurgulayici.getStilBuffer(), 
                          stilTablosu, 
                          sizeof(stilTablosu) / sizeof(stilTablosu[0]), 
                          'A', 0, 0);
```

## Sözdizimi Ağacı

### Düğüm Tipleri

Sözdizimi ağacı, kodun yapısal öğelerini temsil eden düğümlerden oluşur:

```cpp
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
    LITERAL,            // Sabit değer
    IDENTIFIER,         // Tanımlayıcı
    TYPE,               // Veri tipi
    BLOCK_STATEMENT     // Kod bloğu
};
```

### Parser Sınıfı

`Parser` sınıfı, kaynak kodunu okuyup sözdizimi ağacını oluşturur:

```cpp
class Parser {
public:
    Fl_Text_Buffer* buffer;    // Kaynak kod metni
    std::string currentToken;  // Şu anki işlenen token
    size_t position;          // Metin içindeki konum

private:
    // Token işleme metodları
    std::string getNextToken();
    void skipComments();
    bool isType(const std::string& token);
    bool isKeyword(const std::string& token);
    int getOperatorPrecedence(const std::string& op);

    // Ayrıştırma metodları
    std::shared_ptr<ParseNode> parsePrimary();
    std::shared_ptr<ParseNode> parseExpression(int minPrecedence = 0);
    std::shared_ptr<ParseNode> parseAssignment();
    std::shared_ptr<ParseNode> parseVariableDecl();
    std::shared_ptr<ParseNode> parseParameter();
    std::shared_ptr<ParseNode> parseBlock();
    std::shared_ptr<ParseNode> parseStatement();
    std::shared_ptr<ParseNode> parseIfStatement();
    std::shared_ptr<ParseNode> parseWhileStatement();
    std::shared_ptr<ParseNode> parseForStatement();
    std::shared_ptr<ParseNode> parseReturnStatement();
    std::shared_ptr<ParseNode> parseFunctionDef();

public:
    // Constructor
    Parser(Fl_Text_Buffer* buf) : buffer(buf), position(0) {}

    // Ana ayrıştırma metodu
    std::shared_ptr<ParseNode> parse() {
        auto root = std::make_shared<ParseNode>(PROGRAM);
        currentToken = getNextToken();
        while (!currentToken.empty()) {
            if (isType(currentToken)) {
                std::string nextToken = getNextToken();
                position -= nextToken.length();
                if (nextToken == "(") {
                    root->children.push_back(parseFunctionDef());
                } else {
                    root->children.push_back(parseVariableDecl());
                }
            } else if (!isKeyword(currentToken) && currentToken != ";") {
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
```

### Ağaç Yapısı

Her düğüm, kodun bir parçasını ve onun alt öğelerini temsil eder:

```cpp
struct ParseNode {
    NodeType type;      // Düğümün tipi
    std::string value;  // Düğümün değeri (operatör, tanımlayıcı, vb.)
    std::vector<std::shared_ptr<ParseNode>> children;  // Alt düğümler

    // Constructor
    ParseNode(NodeType t, const std::string& v = "") 
        : type(t), value(v) {}
};
```

### Ayrıştırma Stratejisi

Parser, aşağıdaki stratejiyi kullanarak kodu ayrıştırır:

1. **Token Okuma**: Metni token'lara ayırır
2. **Yorum Atlama**: Yorum satırlarını atlar
3. **İfade Ayrıştırma**: İfadeleri öncelik sırasına göre ayrıştırır
4. **Blok Ayrıştırma**: Kod bloklarını ayrıştırır
5. **Fonksiyon Ayrıştırma**: Fonksiyon tanımlarını ayrıştırır

Örnek ayrıştırma kodu:
```cpp
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
```

### Hata Yönetimi

Parser, ayrıştırma sırasında karşılaşılan hataları yönetir:

```cpp
class ParserError : public std::runtime_error {
public:
    ParserError(const std::string& message) 
        : std::runtime_error("Parser Hatası: " + message) {}
};

// Hata kontrolü örneği
void checkToken(const std::string& expected) {
    if (currentToken != expected) {
        throw ParserError("Beklenen token: " + expected + 
                         ", Bulunan token: " + currentToken);
    }
}
```

## Kullanıcı Arayüzü

### Ana Pencere

Ana pencere, kaynak kod düzenleyici ve kontrol butonlarını içerir:

```cpp
class MainWindow : public Fl_Window {
private:
    Fl_Text_Editor *duzenleyici;
    Fl_Text_Buffer *metinBuffer;
    Fl_Button *lexButon;
    Fl_Button *parseButon;
    SyntaxVurgulayici *vurgulayici;
    ParseTreeWindow *parsePencere;
    LexicalAnalizPencere *lexPencere;

public:
    MainWindow(int w, int h, const char* title)
        : Fl_Window(w, h, title) {
        // Düzenleyici oluştur
        duzenleyici = new Fl_Text_Editor(20, 20, 760, 520);
        metinBuffer = new Fl_Text_Buffer();
        duzenleyici->buffer(metinBuffer);

        // Butonları oluştur
        lexButon = new Fl_Button(20, 550, 120, 30, "Lexical Analiz");
        parseButon = new Fl_Button(150, 550, 120, 30, "Sözdizimi Ağacı");

        // Vurgulayıcıyı oluştur
        vurgulayici = new SyntaxVurgulayici(metinBuffer);

        // Pencereleri oluştur
        lexPencere = new LexicalAnalizPencere(400, 600, "Lexical Analiz", 
                                            metinBuffer, vurgulayici);
        parsePencere = new ParseTreeWindow(400, 600, "Sözdizimi Ağacı", 
                                         metinBuffer);

        // Callback'leri ayarla
        lexButon->callback(lex_goster_cb, lexPencere);
        parseButon->callback(parse_tree_cb, parsePencere);
        metinBuffer->add_modify_callback(metin_degisti_cb, vurgulayici);

        end();
    }
};
```

### Lexical Analiz Penceresi

Lexical analiz penceresi, token'ları ağaç yapısında gösterir:

```cpp
class LexicalAnalizPencere : public Fl_Window {
private:
    Fl_Tree *agac;
    Fl_Text_Buffer *metinBuffer;
    SyntaxVurgulayici *vurgulayici;

public:
    LexicalAnalizPencere(int w, int h, const char* title, 
                        Fl_Text_Buffer *buf, SyntaxVurgulayici *vrg)
        : Fl_Window(w, h, title), metinBuffer(buf), vurgulayici(vrg) {
        agac = new Fl_Tree(10, 10, w-20, h-20);
        agac->showroot(0);
        agac->connectorstyle(FL_TREE_CONNECTOR_DOTTED);
        agac->selectmode(FL_TREE_SELECT_NONE);
        end();
    }

    void agaciGuncelle() {
        agac->clear();
        std::string metin = metinBuffer->text();
        if (metin.empty()) return;

        Fl_Tree_Item *kok = agac->add("Kaynak Kod");
        kok->open();

        for (const Token &token : vurgulayici->getTokenlar()) {
            std::string tokenMetni = metin.substr(token.baslangic, 
                                                token.bitis - token.baslangic);
            std::stringstream ss;
            ss << getTokenTipiAdi(token.tip) << ": " << tokenMetni;
            Fl_Tree_Item *oge = agac->add(kok, ss.str().c_str());
            oge->select(0);
        }
    }
};
```

### Sözdizimi Ağacı Penceresi

Sözdizimi ağacı penceresi, kodun yapısal analizini gösterir:

```cpp
class ParseTreeWindow : public Fl_Window {
private:
    Fl_Tree *tree;
    Fl_Text_Buffer *buffer;
    bool isVisible;
    Parser *parser;

    void addNodeToTree(Fl_Tree_Item* parent, 
                      const std::shared_ptr<ParseNode>& node) {
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
    ParseTreeWindow(int w, int h, const char* title, Fl_Text_Buffer *buf)
        : Fl_Window(w, h, "Sözdizimi Ağacı"), buffer(buf), isVisible(false) {
        tree = new Fl_Tree(10, 10, w-20, h-20);
        tree->showroot(0);
        tree->connectorstyle(FL_TREE_CONNECTOR_DOTTED);
        tree->selectmode(FL_TREE_SELECT_NONE);
        parser = new Parser(buffer);
        end();
    }

    void updateTree() {
        if (!isVisible) return;
        tree->clear();
        parser->position = 0;
        auto root = parser->parse();
        Fl_Tree_Item* rootItem = tree->add("Program");
        rootItem->open();
        for (const auto& child : root->children) {
            addNodeToTree(rootItem, child);
        }
        tree->redraw();
    }
};
```

## Örnek Kullanım

### Basit Örnekler

1. **Değişken Tanımlama**:
```cpp
int sayi = 42;
float pi = 3.14159;
string mesaj = "Merhaba";
```

2. **Fonksiyon Tanımlama**:
```cpp
int topla(int a, int b) {
    return a + b;
}
```

3. **Koşul İfadesi**:
```cpp
if (x > 0) {
    return "Pozitif";
} else {
    return "Negatif";
}
```

### Karmaşık Örnekler

1. **İç İçe Fonksiyonlar**:
```cpp
class Matematik {
    float hesapla(int x, float y) {
        if (x > 0 && y < 100.0) {
            float sonuc = 0;
            for (int i = 0; i < x; i++) {
                sonuc += y * i;
            }
            return sonuc;
        }
        return 0.0f;
    }
};
```

2. **Karmaşık Kontrol Yapıları**:
```cpp
void islemYap(int* dizi, int boyut) {
    for (int i = 0; i < boyut; i++) {
        if (dizi[i] > 0) {
            while (dizi[i] > 0) {
                dizi[i]--;
                if (dizi[i] == 0) {
                    break;
                }
            }
        } else {
            do {
                dizi[i]++;
            } while (dizi[i] < 0);
        }
    }
}
```
