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

Bu proje, C kodlarını analiz eden ve görselleştiren iki ana bileşenden oluşmaktadır:

1. **Syntax Vurgulayıcı**: Kaynak kodundaki farklı token'ları (anahtar kelimeler, tanımlayıcılar, sayılar vb.) renklendirerek görsel olarak ayırt edilmesini sağlar.
2. **Sözdizimi Ağacı**: Kaynak kodun yapısal analizini yaparak, kodun hiyerarşik bir ağaç yapısında gösterilmesini sağlar.

Proje FLTK (Fast Light Toolkit) kütüphanesi kullanılarak geliştirilmiştir ve modern bir grafiksel kullanıcı arayüzü sunmaktadır.

### Temel Özellikler

- Gerçek zamanlı syntax vurgulama
- Detaylı lexical analiz görüntüleme
- Hiyerarşik sözdizimi ağacı gösterimi
- Modern ve kullanıcı dostu arayüz

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

Token tipleri, kaynak kodundaki farklı öğeleri sınıflandırmak için kullanılan temel yapı taşlarıdır. Her token tipi, kodun belirli bir öğesini temsil eder ve ona özel bir renk/stil atanır. Bu sınıflandırma sistemi, kodun görsel olarak daha anlaşılır olmasını sağlar. Örneğin; anahtar kelimeler kırmızı renkte, sayılar mavi renkte gösterilir. Bu sayede programcı, kodun farklı bileşenlerini hızlıca ayırt edebilir. Aşağıdaki enum yapısı, tüm olası token tiplerini ve bunların kullanım amaçlarını tanımlar:

```cpp
enum TokenTipi {
    ANAHTAR_KELIME = 0,  // if, else, while gibi C anahtar kelimeleri
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

Token yapısı, her bir token'ın metin içindeki konumunu ve özelliklerini tutan temel veri yapısıdır. Bu yapı token'ın başlangıç ve bitiş pozisyonlarını, tipini ve değerini içerir. Bu bilgiler, syntax vurgulama ve lexical analiz için kritik öneme sahiptir. Örneğin, bir değişken tanımlaması için token yapısı şu bilgileri içerir: başlangıç pozisyonu (değişken adının başladığı yer), bitiş pozisyonu (değişken adının bittiği yer), tip (TANIMLAYICI) ve değer (değişken adı). Bu yapı, aşağıdaki gibi tanımlanır:

```cpp
struct Token {
    int baslangic;       // Token'ın metin içindeki başlangıç pozisyonu
    int bitis;           // Token'ın metin içindeki bitiş pozisyonu
    TokenTipi tip;       // Token'ın tipi
    std::string deger;   // Token'ın değeri (opsiyonel)
};
```

### SyntaxVurgulayici Sınıfı

SyntaxVurgulayici sınıfı, kaynak kodun token'lara ayrılması ve renklendirilmesi işlemlerini yöneten ana sınıftır. Bu sınıf metin buffer'ını, stil buffer'ını ve token listesini tutar. Ayrıca, farklı token tiplerini bulmak için regex desenlerini kullanır. Sınıfın temel görevleri şunlardır:

1. Metin içeriğini sürekli izleme ve değişiklikleri algılama
2. Değişen metni token'lara ayırma
3. Her token'a uygun renk ve stili atama
4. Token listesini güncel tutma
5. Görsel geri bildirim sağlama

Sınıf, aşağıdaki önemli bileşenleri içerir:

```cpp
class SyntaxVurgulayici {
private:
    Fl_Text_Buffer *metinBuffer;           // Düzenlenen metni tutan buffer
    Fl_Text_Buffer *stilBuffer;            // Renk ve stil bilgilerini tutan buffer
    std::vector<Token> tokenlar;           // Bulunan tüm token'ların listesi
    std::map<TokenTipi, Fl_Color> renkHaritasi;  // Token tiplerine göre renk eşleştirmeleri
    std::vector<std::string> anahtarKelimeler;   // C anahtar kelimeleri listesi
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

Regex desenleri, farklı token tiplerini metin içinde bulmak için kullanılan güçlü bir araçtır. Her desen, belirli bir token tipini tanımlar ve eşleşen metinleri bulur. Bu desenler, C kodundaki farklı öğeleri tanımlamak için özel olarak tasarlanmıştır. Örneğin:

- Anahtar kelimeler için desen, C'nin belirli bazı kelimelerini içerir
- Tanımlayıcılar için desen, değişken ve fonksiyon isimlerinin kurallarına uyar
- Sayılar için desen, hem tam sayıları hem de ondalıklı sayıları tanır
- Operatörler için desen, C'nin operatörlerini kapsar
- String'ler için desen, kaçış karakterlerini de dikkate alır

Bu desenler şu şekilde tanımlanır:

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

Token işleme, metni token'lara ayırma ve bu token'ları renklendirme sürecini içeren karmaşık bir işlemdir. Bu süreç iki ana adımdan oluşur:

1. Token Bulma: Metin içindeki token'lar regex desenleri kullanılarak bulunur. Bu adım şunları içerir:
   - Metni karakter karakter tarama
   - Regex desenlerini uygulama
   - Eşleşen token'ları belirleme
   - Token'ların başlangıç ve bitiş pozisyonlarını kaydetme
   - Token'ları tipine göre sınıflandırma

2. Renklendirme: Bulunan token'lar uygun renklerle vurgulanır. Bu adım şunları içerir:
   - Her token tipi için renk ve stil belirleme
   - Metin buffer'ını stil buffer'ı ile eşleştirme
   - Görsel geri bildirimi güncelleme
   - Performans optimizasyonları

İşte bu sürecin nasıl gerçekleştiğini gösteren detaylı kod:

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

### Renk ve Stil Yönetimi

Renk ve stil yönetimi, farklı token tipleri için görsel özellikleri tanımlayan ve uygulayan sistemdir. Her token tipi için özel bir renk ve font stili belirlenir. Bu tanımlamalar, metin düzenleyicide syntax vurgulama için kullanılır. Sistem şu özellikleri içerir:

1. Renk Tanımlamaları:
   - Anahtar kelimeler: Kırmızı (FL_RED)
   - Tanımlayıcılar: Siyah (FL_BLACK)
   - Sayılar: Mavi (FL_BLUE)
   - Operatörler: Koyu Yeşil (FL_DARK_GREEN)
   - String'ler: Mor (FL_MAGENTA)
   - Karakterler: Altın Sarısı (FL_DARKGOLD)
   - Yorumlar: Gri (FL_GRAY)
   - Boşluklar: Beyaz (FL_WHITE)

2. Font Stilleri:
   - Anahtar kelimeler: Kalın (FL_COURIER_BOLD)
   - Yorumlar: İtalik (FL_COURIER_ITALIC)
   - Diğerleri: Normal (FL_COURIER)

3. Font Boyutları:
   - Tüm token'lar için 14 punto

Bu tanımlamalar şu şekilde yapılır:

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

Düğüm tipleri, sözdizimi ağacındaki her bir düğümün ne tür bir kod yapısını temsil ettiğini belirten kapsamlı bir sınıflandırma sistemidir. Bu enum yapısı, programın olası yapısal öğelerini tanımlar ve her bir düğüm tipinin belirli bir kod yapısını temsil etmesini sağlar. Örneğin:

- PROGRAM: Tüm kaynak kodun kök düğümü
- FUNCTION_DEF: Fonksiyon tanımlamaları
- VARIABLE_DECL: Değişken tanımlamaları
- PARAM_LIST: Fonksiyon parametreleri
- STATEMENT: Genel ifadeler
- IF_STATEMENT: Koşul ifadeleri
- WHILE_STATEMENT: Döngü ifadeleri
- EXPRESSION: Matematiksel ve mantıksal ifadeler

Bu yapı şu şekilde tanımlanır:

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

Parser sınıfı, kaynak kodunu okuyup sözdizimi ağacını oluşturan karmaşık bir sistemdir. Bu sınıf token'ları okur, yorumları atlar ve kodun yapısal analizini yapar. Ayrıca, farklı ifade tiplerini ayrıştırmak için özel metodlar içerir. Sınıfın temel özellikleri:

1. Token Yönetimi:
   - Token'ları sırayla okuma
   - Yorum satırlarını atlama
   - Boşlukları işleme
   - Özel karakterleri tanıma

2. Ayrıştırma Stratejisi:
   - Top-Down ayrıştırma
   - Öncelik bazlı ifade ayrıştırma
   - Hata tespiti ve raporlama

3. Ağaç Oluşturma:
   - Düğümleri hiyerarşik olarak oluşturma
   - İlişkileri belirleme
   - Ağaç yapısını optimize etme

Bu sınıf şu şekilde tanımlanır:

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

Ağaç yapısı, kodun hiyerarşik temsilini sağlayan temel veri yapısıdır. Her düğüm, kodun bir parçasını ve onun alt öğelerini temsil eder. Bu yapı, kodun yapısal analizini görselleştirmek için kullanılır. Ağaç yapısının özellikleri:

1. Düğüm Özellikleri:
   - Tip bilgisi (NodeType)
   - Değer bilgisi (string)
   - Alt düğümler listesi
   - Ebeveyn düğüm referansı

2. Hiyerarşik İlişkiler:
   - Program -> Fonksiyonlar -> İfadeler
   - İfadeler -> Alt ifadeler
   - Bloklar -> İçerikler

3. Veri Yönetimi:
   - Akıllı işaretçiler (shared_ptr)
   - Otomatik bellek yönetimi
   - Verimli veri yapısı

Bu yapı şu şekilde tanımlanır:

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

Ayrıştırma stratejisi, kodun nasıl ayrıştırılacağını ve sözdizimi ağacının nasıl oluşturulacağını tanımlayan kapsamlı bir sistemdir. Bu süreç token okuma, yorum atlama, ifade ayrıştırma, blok ayrıştırma ve fonksiyon ayrıştırma adımlarını içerir. Stratejinin ana bileşenleri:

1. Token İşleme:
   - Token'ları sırayla okuma
   - Geçerli token'ı takip etme
   - Token tipini belirleme
   - Token değerini işleme

2. İfade Ayrıştırma:
   - Öncelik bazlı ayrıştırma
   - Operatör önceliklerini dikkate alma
   - Parantez içi ifadeleri işleme
   - Karmaşık ifadeleri basitleştirme

3. Blok Ayrıştırma:
   - Süslü parantez içi kodları işleme
   - İç içe blokları yönetme
   - Kapsam kurallarını uygulama
   - Değişken erişimini kontrol etme

4. Fonksiyon Ayrıştırma:
   - Fonksiyon başlığını işleme
   - Parametreleri ayrıştırma
   - Dönüş tipini belirleme
   - Fonksiyon gövdesini analiz etme

Bu strateji şu şekilde uygulanır:

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

Hata yönetimi, ayrıştırma sırasında karşılaşılan hataları yakalayan ve işleyen bir sistemdir. Bu sistem, beklenmeyen token'lar veya geçersiz kod yapıları için özel hata mesajları üretir. Hata yönetiminin özellikleri:

1. Hata Türleri:
   - Sözdizimi hataları
   - Eksik parantezler
   - Geçersiz ifadeler
   - Tip uyumsuzlukları
   - Tanımlanmamış değişkenler

2. Hata İşleme:
   - Hata tespiti
   - Hata mesajı oluşturma
   - Hata konumunu belirleme
   - Kullanıcıya bildirim
   - Kurtarma mekanizmaları

3. Hata Raporlama:
   - Detaylı hata açıklamaları
   - Satır numarası belirtme
   - Beklenen ve bulunan değerleri gösterme
   - Öneriler sunma

Bu sistem şu şekilde uygulanır:

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

Ana pencere, uygulamanın temel arayüzünü oluşturan bileşendir. Bu sınıf metin düzenleyici, kontrol butonları ve diğer UI bileşenlerini içerir. Ayrıca, kullanıcı etkileşimlerini yönetir. Pencere özellikleri:

1. Bileşenler:
   - Metin düzenleyici (Fl_Text_Editor)
   - Lexical analiz butonu
   - Sözdizimi ağacı butonu
   - Durum çubuğu
   - Menü çubuğu

2. Düzen:
   - Esnek yerleşim
   - Yeniden boyutlandırma desteği
   - Responsive tasarım
   - Kullanıcı dostu arayüz

3. İşlevsellik:
   - Gerçek zamanlı syntax vurgulama
   - Dosya işlemleri
   - Analiz araçları
   - Ayarlar yönetimi

Bu pencere şu şekilde tanımlanır:

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

Lexical analiz penceresi, token'ları ağaç yapısında gösteren özel bir görüntüleme bileşenidir. Bu pencere, kaynak kodun lexical analizini görselleştirir ve her token'ın tipini ve değerini gösterir. Pencere özellikleri:

1. Görüntüleme:
   - Token ağacı
   - Token detayları
   - Renk kodlaması
   - Genişletilebilir düğümler

2. İnteraktif Özellikler:
   - Düğüm seçimi
   - Detay görüntüleme
   - Arama ve filtreleme

3. Güncelleme:
   - Gerçek zamanlı güncelleme
   - Değişiklik takibi
   - Performans optimizasyonu
   - Bellek yönetimi

Bu pencere şu şekilde tanımlanır:

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

Sözdizimi ağacı penceresi, kodun yapısal analizini gösteren gelişmiş bir görselleştirme aracıdır. Bu pencere, kodun hiyerarşik yapısını ağaç görünümünde sunar ve her düğümün tipini ve değerini gösterir. Pencere özellikleri:

1. Ağaç Görünümü:
   - Hiyerarşik yapı
   - Düğüm detayları
   - İlişki gösterimi
   - Görsel stil

2. İnteraktif Özellikler:
   - Düğüm genişletme/daraltma
   - Düğüm seçimi
   - Detay görüntüleme
   - Arama ve filtreleme

3. Güncelleme Mekanizması:
   - Gerçek zamanlı güncelleme
   - Değişiklik takibi
   - Performans optimizasyonu
   - Bellek yönetimi

Bu pencere şu şekilde tanımlanır:

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

Aşağıdaki örnekler, programın temel özelliklerini göstermek için kullanılabilir. Her örnek, farklı bir kod yapısını ve onun nasıl analiz edildiğini gösterir. Bu örnekler, programın temel işlevselliğini anlamak için idealdir:

1. Değişken Tanımlamaları:
   - Farklı veri tipleri
   - İlk değer atamaları
   - Tip dönüşümleri

2. Fonksiyon Tanımlamaları:
   - Parametre listeleri
   - Dönüş tipleri
   - Fonksiyon gövdeleri

3. Kontrol Yapıları:
   - If-else ifadeleri
   - Döngüler
   - Switch-case yapıları

Örnek kodlar:

```cpp
int sayi = 42;
float pi = 3.14159;
string mesaj = "Merhaba";

int topla(int a, int b) {
    return a + b;
}

if (x > 0) {
    return "Pozitif";
} else {
    return "Negatif";
}
```

### Karmaşık Örnekler

Karmaşık örnekler, programın daha gelişmiş özelliklerini gösterir. Bu örnekler iç içe fonksiyonlar, karmaşık kontrol yapıları ve diğer ileri düzey kod yapılarını içerir. Bu örnekler, programın güçlü yönlerini ve karmaşık kod yapılarını nasıl analiz edebildiğini gösterir:

1. İç İçe Yapılar:
   - Sınıf tanımlamaları
   - İç içe fonksiyonlar
   - Karmaşık ifadeler

2. Gelişmiş Kontrol Yapıları:
   - Çoklu koşullar
   - İç içe döngüler
   - Karmaşık mantık

3. Özel Durumlar:
   - Şablonlar
   - Kalıtım
   - Çoklu kalıtım

Örnek kodlar:

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
