#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Button.H>
#include <vector>
#include <string>
#include <regex>
#include <map>
#include <sstream>
#include "FL/forms.H"
#include "parse_tree.cpp"


void metin_degisti_parse_cb(int pos, int nInserted, int nDeleted, int, const char *, void *v);
void parse_tree_cb(Fl_Widget*, void* v);

// Token tiplerini tanımlayan enum
// Her token tipi için bir renk ve stil atanacak
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

// Her token için gerekli bilgileri tutan yapı
struct Token {
    int baslangic;       // Token'ın metin içindeki başlangıç pozisyonu
    int bitis;           // Token'ın metin içindeki bitiş pozisyonu
    TokenTipi tip;       // Token'ın tipi
};

// Syntax vurgulama işlemlerini yapan ana sınıf
class SyntaxVurgulayici {
private:
    Fl_Text_Buffer *metinBuffer;           // Düzenlenen metni tutan buffer
    Fl_Text_Buffer *stilBuffer;            // Renk ve stil bilgilerini tutan buffer
    std::vector<Token> tokenlar;           // Bulunan tüm token'ların listesi
    std::map<TokenTipi, Fl_Color> renkHaritasi;  // Token tiplerine göre renk eşleştirmeleri
    // C++ anahtar kelimeleri listesi
    std::vector<std::string> anahtarKelimeler = {"if", "else", "while", "for", "return", "int", "float", "void", "char", "string"};
    // Token tiplerini bulmak için kullanılan regex desenleri
    std::regex anahtarKelimeRegex;    // Anahtar kelimeleri bulmak için
    std::regex tanimlayiciRegex;      // Değişken/fonksiyon isimlerini bulmak için
    std::regex sayiRegex;             // Sayıları bulmak için
    std::regex operatorRegex;         // Operatörleri bulmak için
    std::regex metinRegex;            // String'leri bulmak için
    std::regex karakterRegex;         // Karakterleri bulmak için
    std::regex yorumRegex;            // Yorumları bulmak için

public:
    // Constructor: Buffer'ı alır ve regex desenlerini hazırlar
    SyntaxVurgulayici(Fl_Text_Buffer *buf) : metinBuffer(buf) {
        // Her token tipi için renk tanımlamaları
        renkHaritasi = {
            {ANAHTAR_KELIME, FL_RED},        // Anahtar kelimeler kırmızı
            {TANIMLAYICI, FL_BLACK},         // Tanımlayıcılar siyah
            {SAYI, FL_BLUE},                 // Sayılar mavi
            {OPERATOR, FL_DARK_GREEN},       // Operatörler koyu yeşil
            {METIN, FL_MAGENTA},             // String'ler mor
            {KARAKTER, FL_DARKGOLD},        // Karakterler altın sarısı
            {YORUM, FL_GRAY},                // Yorumlar gri
            {BOSLUK, FL_WHITE}               // Boşluklar beyaz
        };
        // Regex desenlerini hazırla
        anahtarKelimeRegex = std::regex("\\b(" + anahtarKelimeDeseniAl() + ")\\b");
        tanimlayiciRegex = std::regex("[a-zA-Z_]\\w*");  // Harf veya _ ile başlayan kelimeler
        sayiRegex = std::regex("\\b\\d+(\\.\\d+)?\\b");  // Tam sayı veya ondalıklı sayı
        operatorRegex = std::regex("[\\+\\-\\*/%=&|<>!]");  // Temel operatörler
        metinRegex = std::regex("\"(?:[^\"\\\\]|\\\\.)*\"");  // Çift tırnak içindeki metinler
        karakterRegex = std::regex("'(?:[^'\\\\]|\\\\.)'");  // Tek tırnak içindeki karakterler
        yorumRegex = std::regex("(//[^\\n]*)|(/\\*[\\s\\S]*?\\*/)");
        stilBuffer = new Fl_Text_Buffer();
    }

    // Anahtar kelimeleri regex desenine dönüştürür
    std::string anahtarKelimeDeseniAl() {
        std::string desen;
        for (size_t i = 0; i < anahtarKelimeler.size(); ++i) {
            if (i != 0) desen += "|";
            desen += anahtarKelimeler[i];
        }
        return desen;
    }

    // Metni token'lara ayırır
    void tokenize() {
        tokenlar.clear();
        std::string metin = metinBuffer->text();
        if (metin.empty()) return;
        // Önce string'leri bul ve işaretle
        std::vector<std::pair<int, int>> stringAraliklari;
        auto string_baslangic = std::sregex_iterator(metin.begin(), metin.end(), metinRegex);
        auto string_bitis = std::sregex_iterator();

        for (std::sregex_iterator i = string_baslangic; i != string_bitis; ++i) {
            std::smatch eslesme = *i;
            stringAraliklari.push_back({eslesme.position(), eslesme.position() + eslesme.length()});
            Token token;
            token.baslangic = eslesme.position();
            token.bitis = token.baslangic + eslesme.length();
            token.tip = METIN;
            tokenlar.push_back(token);
        }
        // Karakterleri bul ve işaretle
        std::vector<std::pair<int, int>> karakterAraliklari;
        auto karakter_baslangic = std::sregex_iterator(metin.begin(), metin.end(), karakterRegex);
        auto karakter_bitis = std::sregex_iterator();
        for (std::sregex_iterator i = karakter_baslangic; i != karakter_bitis; ++i) {
            std::smatch eslesme = *i;
            karakterAraliklari.push_back({eslesme.position(), eslesme.position() + eslesme.length()});
            Token token;
            token.baslangic = eslesme.position();
            token.bitis = token.baslangic + eslesme.length();
            token.tip = KARAKTER;
            tokenlar.push_back(token);
        }
        // Sonra yorumları bul ve işaretle
        std::vector<std::pair<int, int>> yorumAraliklari;
        auto yorum_baslangic = std::sregex_iterator(metin.begin(), metin.end(), yorumRegex);
        auto yorum_bitis = std::sregex_iterator();
        for (std::sregex_iterator i = yorum_baslangic; i != yorum_bitis; ++i) {
            std::smatch eslesme = *i;
            yorumAraliklari.push_back({eslesme.position(), eslesme.position() + eslesme.length()});
            Token token;
            token.baslangic = eslesme.position();
            token.bitis = token.baslangic + eslesme.length();
            token.tip = YORUM;
            tokenlar.push_back(token);
        }
        // Diğer token'ları bul, ama string, karakter ve yorum içindeki kısımları atla
        auto eslesmeKontrol = [&stringAraliklari, &karakterAraliklari, &yorumAraliklari]
                             (int baslangic, int bitis) -> bool {
            // String içinde mi kontrol et
            for (const auto& str : stringAraliklari) {
                if (baslangic >= str.first && bitis <= str.second) {
                    return true;
            }
            }
            // Karakter içinde mi kontrol et
            for (const auto& kar : karakterAraliklari) {
                if (baslangic >= kar.first && bitis <= kar.second) {
                    return true;
        }
    }
            // Yorum içinde mi kontrol et
            for (const auto& yorum : yorumAraliklari) {
                if (baslangic >= yorum.first && bitis <= yorum.second) {
                    return true;
                }
            }
            return false;
        };
        // Anahtar kelimeleri bul
        auto kelime_baslangic = std::sregex_iterator(metin.begin(), metin.end(), anahtarKelimeRegex);
        auto kelime_bitis = std::sregex_iterator();
        for (std::sregex_iterator i = kelime_baslangic; i != kelime_bitis; ++i) {
            std::smatch eslesme = *i;
            if (!eslesmeKontrol(eslesme.position(), eslesme.position() + eslesme.length())) {
                Token token;
                token.baslangic = eslesme.position();
                token.bitis = token.baslangic + eslesme.length();
                token.tip = ANAHTAR_KELIME;
                tokenlar.push_back(token);
            }
        }
        // Sayıları bul
        auto sayi_baslangic = std::sregex_iterator(metin.begin(), metin.end(), sayiRegex);
        auto sayi_bitis = std::sregex_iterator();
        for (std::sregex_iterator i = sayi_baslangic; i != sayi_bitis; ++i) {
            std::smatch eslesme = *i;
            if (!eslesmeKontrol(eslesme.position(), eslesme.position() + eslesme.length())) {
                Token token;
                token.baslangic = eslesme.position();
                token.bitis = token.baslangic + eslesme.length();
                token.tip = SAYI;
                tokenlar.push_back(token);
            }
        }
        // Operatörleri bul
        auto operator_baslangic = std::sregex_iterator(metin.begin(), metin.end(), operatorRegex);
        auto operator_bitis = std::sregex_iterator();
        for (std::sregex_iterator i = operator_baslangic; i != operator_bitis; ++i) {
            std::smatch eslesme = *i;
            if (!eslesmeKontrol(eslesme.position(), eslesme.position() + eslesme.length())) {
                Token token;
                token.baslangic = eslesme.position();
                token.bitis = token.baslangic + eslesme.length();
                token.tip = OPERATOR;
                tokenlar.push_back(token);
            }
        }
        // Tanımlayıcıları bul
        auto tanimlayici_baslangic = std::sregex_iterator(metin.begin(), metin.end(), tanimlayiciRegex);
        auto tanimlayici_bitis = std::sregex_iterator();
        for (std::sregex_iterator i = tanimlayici_baslangic; i != tanimlayici_bitis; ++i) {
            std::smatch eslesme = *i;
            if (!eslesmeKontrol(eslesme.position(), eslesme.position() + eslesme.length())) {
                bool anahtarKelimeMi = std::regex_match(eslesme.str(), anahtarKelimeRegex);
                if (!anahtarKelimeMi) {
                    Token token;
                    token.baslangic = eslesme.position();
                    token.bitis = token.baslangic + eslesme.length();
                    token.tip = TANIMLAYICI;
                    tokenlar.push_back(token);
                }
            }
        }
        // Token'ları başlangıç pozisyonuna göre sırala
        std::sort(tokenlar.begin(), tokenlar.end(),
                 [](const Token &a, const Token &b) { return a.baslangic < b.baslangic; });
    }
    // Bulunan token'lara göre metni renklendirir
    void vurgula() {
        if (metinBuffer->length() == 0) return;
        // Önce tüm metni tanımlayıcı rengiyle doldur
        std::string stil(metinBuffer->length(), 'A' + TANIMLAYICI);
        stilBuffer->text(stil.c_str());
        // Her token için uygun rengi uygula
        for (const Token &token : tokenlar) {
            if (token.tip == BOSLUK) continue;  // Boşlukları atla
            char stilKarakteri = 'A' + token.tip;
            for (int i = token.baslangic; i < token.bitis; ++i) {
                stilBuffer->replace(i, i + 1, &stilKarakteri, 1);
            }
        }
    }
    // Stil buffer'ını dışarıya aç
    Fl_Text_Buffer *getStilBuffer() { return stilBuffer; }
    // Token listesini dışarıya aç
    const std::vector<Token>& getTokenlar() const { return tokenlar; }
};

// Lexical analiz gösteren pencere sınıfı
class LexicalAnalizPencere : public Fl_Window {
private:
    Fl_Tree *agac;                    // Token'ları gösteren ağaç yapısı
    Fl_Text_Buffer *metinBuffer;      // Kaynak metin buffer'ı
    SyntaxVurgulayici *vurgulayici;   // Syntax vurgulayıcı referansı

public:
    // Constructor: Pencereyi ve ağaç yapısını hazırlar
    LexicalAnalizPencere(int genislik, int yukseklik, const char* baslik, Fl_Text_Buffer *buf, SyntaxVurgulayici *vrg)
        : Fl_Window(genislik, yukseklik, baslik), metinBuffer(buf), vurgulayici(vrg) {
        agac = new Fl_Tree(10, 10, genislik-20, yukseklik-20);
        agac->showroot(0);                    // Kök düğümü gizle
        agac->connectorstyle(FL_TREE_CONNECTOR_DOTTED);  // Noktalı bağlantı çizgileri
        agac->selectmode(FL_TREE_SELECT_NONE);  // Seçimi devre dışı bırak
        end();
    }

    // Ağacı güncel token'larla yeniler
    void agaciGuncelle() {
        agac->clear();
        std::string metin = metinBuffer->text();
        if (metin.empty()) return;
        // Kök düğümü ekle
        Fl_Tree_Item *kok = agac->add("Kaynak Kod");
        kok->open();  // Kök düğümü açık başlat
        // Her token'ı ağaca ekle
        for (const Token &token : vurgulayici->getTokenlar()) {
            std::string tokenMetni = metin.substr(token.baslangic, token.bitis - token.baslangic);
            std::stringstream ss;
            ss << getTokenTipiAdi(token.tip) << ": " << tokenMetni;
            Fl_Tree_Item *oge = agac->add(kok, ss.str().c_str());
            oge->select(0);  // Seçimi devre dışı bırak
        }
    }

    // Token tipini Türkçe isme çevirir
    static const char* getTokenTipiAdi(TokenTipi tip) {
        switch (tip) {
            case ANAHTAR_KELIME: return "Anahtar Kelime";
            case TANIMLAYICI: return "Tanimlayici";
            case SAYI: return "Sayi";
            case OPERATOR: return "Operator";
            case METIN: return "Metin";
            case KARAKTER: return "Karakter";
            case YORUM: return "Yorum";
            case BOSLUK: return "Bosluk";
            default: return "Bilinmeyen";
        }
    }
};

// Lexical analiz penceresini gösteren callback
void lex_goster_cb(Fl_Widget*, void* v) {
    LexicalAnalizPencere *lexPencere = static_cast<LexicalAnalizPencere*>(v);
    lexPencere->agaciGuncelle();
    lexPencere->show();
}

// Metin değiştiğinde çağrılan callback
void metin_degisti_cb(int pos, int nInserted, int nDeleted, int, const char *, void *v) {
    if (nInserted || nDeleted) {
        SyntaxVurgulayici *vurgulayici = static_cast<SyntaxVurgulayici *>(v);
        vurgulayici->tokenize();
        vurgulayici->vurgula();
    }
}

int main() {
    // Ana pencereyi oluştur
    Fl_Window *pencere = new Fl_Window(800, 600, "C++ Syntax Vurgulayici");
    // Metin düzenleyiciyi oluştur
    Fl_Text_Editor *duzenleyici = new Fl_Text_Editor(20, 20, 760, 520);
    Fl_Text_Buffer *metinBuffer = new Fl_Text_Buffer();
    duzenleyici->buffer(metinBuffer);
    
    // Lexical analiz butonunu oluştur
    Fl_Button *lexButon = new Fl_Button(20, 550, 120, 30, "Lexical Analiz");
    // Parse Tree butonunu oluştur
    Fl_Button *parseButon = new Fl_Button(150, 550, 120, 30, "Sözdizimi Ağacı");
    
    // Syntax vurgulayıcıyı oluştur
    SyntaxVurgulayici vurgulayici(metinBuffer);
    
    // Lexical Analiz penceresini oluştur
    LexicalAnalizPencere *lexPencere = new LexicalAnalizPencere(400, 600, "Lexical Analiz", metinBuffer, &vurgulayici);
    // Parse Tree penceresini oluştur
    ParseTreeWindow *parsePencere = new ParseTreeWindow(400, 600, "Sözdizimi Ağacı", metinBuffer);
    
    // Callback'leri ayarla
    lexButon->callback(lex_goster_cb, lexPencere);
    parseButon->callback(parse_tree_cb, parsePencere);
    metinBuffer->add_modify_callback(metin_degisti_cb, &vurgulayici);
    // Stil tablosunu tanımla
    Fl_Text_Display::Style_Table_Entry stilTablosu[] = {
        { FL_RED,        FL_COURIER_BOLD, 14 },    // A - ANAHTAR_KELIME
        { FL_BLACK,      FL_COURIER, 14 },         // B - TANIMLAYICI
        { FL_BLUE,       FL_COURIER, 14 },         // C - SAYI
        { FL_DARK_GREEN, FL_COURIER, 14 },         // D - OPERATOR
        { FL_MAGENTA,    FL_COURIER, 14 },         // E - METIN
        { FL_DARKGOLD,  FL_COURIER, 14 },         // F - KARAKTER
        { FL_GRAY,       FL_COURIER_ITALIC, 14 },  // G - YORUM
        { FL_WHITE,      FL_COURIER, 14 }         // H - BOSLUK
    };
    // Düzenleyiciye stil tablosunu uygula
    duzenleyici->highlight_data(vurgulayici.getStilBuffer(), stilTablosu, sizeof(stilTablosu) / sizeof(stilTablosu[0]), 'A', 0, 0);
    // Örnek kod
    metinBuffer->text(
        "// Bu bir yorum satırıdır\n"
        "/* Bu da\n"
        "   çok satırlı\n"
        "   bir yorumdur */\n\n"
        "string mesaj = \"Merhaba Dünya!\";\n"
        "int sayi = 42;\n"
        "float pi = 3.14159;\n"
        "char karakter = 'A';\n\n"
        "string hesapla(int x, float y) {\n"
        "    if (x > 0 && y < 100.0) {\n"
        "        string sonuc = \"Pozitif\";\n"
        "        return sonuc;\n"
        "    } else {\n"
        "        while (x != 0) {\n"
        "            x = x - 1;\n"
        "            y = y * 2.0;\n"
        "        }\n"
        "        for (int i = 0; i < 10; i++) {\n"
        "            float toplam = x + y;\n"
        "            if (toplam >= 50.0) {\n"
        "                break;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "    return \"Bitti\";\n"
        "}\n\n"
    );
    // Pencereyi göster ve uygulamayı başlat
    pencere->end();
    pencere->show();
    // İlk token'ları bul ve vurgula
    vurgulayici.tokenize();
    vurgulayici.vurgula();
    return Fl::run();
}
