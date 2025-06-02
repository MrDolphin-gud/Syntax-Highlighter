# API Dokümantasyonu

Bu dokümantasyon, Sözdizimi Ağacı Görselleştirici'nin temel bileşenlerini ve API'lerini detaylı olarak açıklar.

## ParseNode Yapısı

```cpp
struct ParseNode {
    NodeType type;      // Düğümün tipi
    std::string value;  // Düğümün değeri
    std::vector<std::shared_ptr<ParseNode>> children;  // Alt düğümler
};
```

### Açıklama
`ParseNode` yapısı, sözdizimi ağacının temel yapı taşıdır. Her düğüm:
- Bir `NodeType` ile tipini belirtir
- Opsiyonel bir değer içerebilir (örn. değişken adı, operatör)
- Alt düğümlerini bir vektör içinde tutar

## Parser Sınıfı

### Yapıcı
```cpp
Parser(Fl_Text_Buffer* buf)
```
- `buf`: Analiz edilecek kaynak kodun bulunduğu metin tamponu

### Ana Metodlar

#### parse()
```cpp
std::shared_ptr<ParseNode> parse()
```
Kaynak kodu ayrıştırır ve sözdizimi ağacını oluşturur.

#### parseFunctionDef()
```cpp
std::shared_ptr<ParseNode> parseFunctionDef()
```
Fonksiyon tanımlarını ayrıştırır. Örnek:
```cpp
int topla(int a, int b) { return a + b; }
```

#### parseVariableDecl()
```cpp
std::shared_ptr<ParseNode> parseVariableDecl()
```
Değişken tanımlamalarını ayrıştırır. Örnek:
```cpp
int sayi = 42;
```

#### parseStatement()
```cpp
std::shared_ptr<ParseNode> parseStatement()
```
İfadeleri ayrıştırır. Desteklenen ifadeler:
- If-else koşulları
- While döngüleri
- For döngüleri
- Return ifadeleri
- Değişken tanımlamaları
- Atama ifadeleri

#### parseExpression()
```cpp
std::shared_ptr<ParseNode> parseExpression(int minPrecedence = 0)
```
İfadeleri ve operatörleri ayrıştırır. Operatör önceliğini dikkate alır.

## ParseTreeWindow Sınıfı

### Yapıcı
```cpp
ParseTreeWindow(int w, int h, const char* title, Fl_Text_Buffer* buf)
```
- `w`: Pencere genişliği
- `h`: Pencere yüksekliği
- `title`: Pencere başlığı
- `buf`: Kaynak kod metni

### Ana Metodlar

#### updateTree()
```cpp
void updateTree()
```
Sözdizimi ağacını günceller. Kaynak kod değiştiğinde otomatik olarak çağrılır.

#### show()
```cpp
void show()
```
Ağaç görünümü penceresini gösterir.

#### hide()
```cpp
void hide()
```
Ağaç görünümü penceresini gizler.

## Yardımcı Fonksiyonlar

### metin_degisti_parse_cb()
```cpp
void metin_degisti_parse_cb(int pos, int nInserted, int nDeleted, int, const char*, void* v)
```
Metin değişikliklerini izleyen callback fonksiyonu. Metin değiştiğinde ağacı günceller.

### parse_tree_cb()
```cpp
void parse_tree_cb(Fl_Widget*, void* v)
```
Ağaç görünümü penceresini açan callback fonksiyonu.

## Hata İşleme

Parser sınıfı, aşağıdaki durumlarda hata toleranslı davranır:
- Eksik noktalı virgüller
- Eksik parantezler
- Geçersiz ifadeler

Hata durumunda, mümkün olduğunca ayrıştırmaya devam eder ve geçerli kısımları ağaca ekler.

## Performans Notları

- Ağaç güncellemeleri gerçek zamanlı olarak yapılır
- Büyük dosyalarda performans için aşağıdaki optimizasyonlar kullanılır:
  - Gereksiz ifade düğümlerinin temizlenmesi
  - Token önbelleğe alma
  - Verimli ağaç yapısı

## Örnek Kullanım

```cpp
// Metin tamponu oluştur
Fl_Text_Buffer* buffer = new Fl_Text_Buffer();

// Parser oluştur
Parser parser(buffer);

// Ağaç penceresi oluştur
ParseTreeWindow* window = new ParseTreeWindow(800, 600, "Sözdizimi Ağacı", buffer);

// Kaynak kodu ayarla
buffer->text("int main() { return 0; }");

// Ağacı güncelle
window->updateTree();

// Pencereyi göster
window->show();
``` 