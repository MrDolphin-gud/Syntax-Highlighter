# Sözdizimi Ağacı Oluşturucu - Detaylı Dokümantasyon

## İçindekiler
1. [Genel Bakış](#genel-bakış)
2. [Kod Yapısı](#kod-yapısı)
3. [Veri Yapıları](#veri-yapıları)
4. [Sınıflar ve Bileşenler](#sınıflar-ve-bileşenler)
5. [Ayrıştırma Süreci](#ayrıştırma-süreci)
6. [Görselleştirme](#görselleştirme)
7. [Kullanım](#kullanım)

## Genel Bakış

Bu kod, C++ programlama dilinde yazılmış bir sözdizimi ağacı (parse tree) oluşturucu ve görselleştiricidir. FLTK (Fast Light Toolkit) kütüphanesi kullanılarak geliştirilmiş olup, kaynak kodunu ayrıştırarak sözdizimi ağacını oluşturur ve görsel bir arayüz üzerinde gösterir.

### Temel Özellikler
- Kaynak kodunu token'lara ayırma
- Sözdizimi ağacı oluşturma
- Ağacı görsel olarak gösterme
- Gerçek zamanlı güncelleme
- C++ benzeri dil yapılarını destekleme

## Kod Yapısı

Kod, aşağıdaki ana bileşenlerden oluşmaktadır:

### Başlık Dosyaları
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Text_Buffer.H>
#include <string>
#include <vector>
#include <memory>
```

Bu başlık dosyaları, programın temel işlevselliği için gerekli kütüphaneleri içerir:
- FLTK bileşenleri (pencere, ağaç görünümü, metin tamponu)
- Standart C++ kütüphaneleri (string, vector, memory)

## Veri Yapıları

### NodeType (Düğüm Tipleri)
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
    BINARY_EXPR,        // İkili işlem
    UNARY_EXPR,         // Tekli işlem
    ASSIGNMENT_EXPR,    // Atama ifadesi
    LITERAL,            // Sabit değer
    IDENTIFIER,         // Tanımlayıcı
    TYPE,               // Veri tipi
    BLOCK_STATEMENT     // Kod bloğu
};
```

### ParseNode (Ayrıştırma Düğümü)
```cpp
struct ParseNode {
    NodeType type;      // Düğümün tipi
    std::string value;  // Düğümün değeri
    std::vector<std::shared_ptr<ParseNode>> children;  // Alt düğümler
};
```

## Sınıflar ve Bileşenler

### 1. Parser Sınıfı

Parser sınıfı, kaynak kodunu ayrıştırmaktan ve sözdizimi ağacını oluşturmaktan sorumludur.

#### Önemli Metodlar:
- `parse()`: Ana ayrıştırma metodu
- `getNextToken()`: Bir sonraki token'ı alır
- `parseExpression()`: İfadeleri ayrıştırır
- `parseStatement()`: İfadeleri ayrıştırır
- `parseFunctionDef()`: Fonksiyon tanımlarını ayrıştırır

#### Yardımcı Metodlar:
- `isType()`: Token'ın veri tipi olup olmadığını kontrol eder
- `isKeyword()`: Token'ın anahtar kelime olup olmadığını kontrol eder
- `getOperatorPrecedence()`: Operatör önceliğini döndürür
- `skipComments()`: Yorum satırlarını atlar

### 2. ParseTreeWindow Sınıfı

ParseTreeWindow sınıfı, sözdizimi ağacını görsel olarak göstermekten sorumludur.

#### Önemli Özellikler:
- FLTK tabanlı pencere yönetimi
- Ağaç görünümü (Fl_Tree)
- Gerçek zamanlı güncelleme
- Görselleştirme kontrolleri

#### Önemli Metodlar:
- `updateTree()`: Ağacı günceller
- `addNodeToTree()`: Düğümleri ağaca ekler
- `show()/hide()`: Pencere görünürlüğünü kontrol eder

## Ayrıştırma Süreci

### 1. Token Ayrıştırma
- Boşluklar ve yorumlar atlanır
- Tanımlayıcılar, sayılar, metinler ve operatörler ayrıştırılır
- Her token tipi için özel işleme kuralları uygulanır

### 2. Sözdizimi Ağacı Oluşturma
1. Program kök düğümü oluşturulur
2. Her ifade için uygun düğüm tipi belirlenir
3. Alt düğümler oluşturulur ve bağlanır
4. Ağaç yapısı hiyerarşik olarak oluşturulur

### 3. İfade Ayrıştırma Sırası
1. Temel ifadeler (tanımlayıcılar, sabitler)
2. İkili işlemler (operatörler)
3. Atama ifadeleri
4. Kontrol yapıları (if, while, for)
5. Fonksiyon tanımları

## Görselleştirme

### Ağaç Görünümü Özellikleri
- Noktalı bağlantı çizgileri
- Düğüm tipleri için renk kodlaması
- Genişletilebilir/daraltılabilir düğümler
- Otomatik düzen

### Güncelleme Mekanizması
- Metin değişikliklerinde otomatik güncelleme
- Callback fonksiyonları ile tetikleme
- Verimli yeniden çizim

## Kullanım

### Temel Kullanım
1. Parser nesnesi oluşturulur
2. Kaynak kod metni yüklenir
3. Ayrıştırma işlemi başlatılır
4. Sonuçlar görselleştirilir

### Örnek Kod
```cpp
// Parser oluşturma
Fl_Text_Buffer* buffer = new Fl_Text_Buffer();
Parser parser(buffer);

// Pencere oluşturma
ParseTreeWindow* window = new ParseTreeWindow(800, 600, "Sözdizimi Ağacı", buffer);

// Ağacı güncelleme
window->updateTree();

// Pencereyi gösterme
window->show();
```

### Callback Fonksiyonları
- `metin_degisti_parse_cb`: Metin değişikliklerini izler
- `parse_tree_cb`: Ağaç görünümünü tetikler

## Hata Yönetimi

### Olası Hatalar ve Çözümleri
1. Geçersiz sözdizimi
   - Hata mesajları gösterilir
   - Ayrıştırma işlemi güvenli şekilde devam eder

2. Bellek yönetimi
   - Smart pointer'lar kullanılır
   - Kaynaklar otomatik temizlenir

3. Görselleştirme hataları
   - Pencere yeniden boyutlandırılabilir
   - Ağaç görünümü otomatik güncellenir

## Performans Özellikleri

### Optimizasyonlar
- Verimli token ayrıştırma
- Akıllı bellek yönetimi
- Hızlı ağaç güncelleme
- Minimum yeniden çizim

### Sınırlamalar
- Büyük dosyalar için bellek kullanımı
- Karmaşık ifadelerde ayrıştırma süresi
- Görselleştirme performansı

## Geliştirme ve Genişletme

### Yeni Özellik Ekleme
1. Yeni düğüm tipleri eklemek için:
   - NodeType enum'una yeni tip ekle
   - İlgili ayrıştırma metodlarını güncelle
   - Görselleştirme kodunu güncelle

2. Yeni dil yapıları eklemek için:
   - Parser sınıfına yeni ayrıştırma metodları ekle
   - Token tanıma kurallarını güncelle
   - Ağaç oluşturma mantığını genişlet

### Bakım ve Güncelleme
- Düzenli kod temizliği
- Bellek sızıntılarını önleme
- Performans optimizasyonları
- Hata düzeltmeleri 