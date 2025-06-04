# C Kod Analiz Aracı

Bu proje, C kaynak kodunu analiz eden ve görselleştiren bir araçtır. Kodun sözdizimi analizini yaparak, lexical analiz ve parse tree (sözdizimi ağacı) görselleştirmesi sunar.

## Özellikler

### 1. Syntax Vurgulama
- C anahtar kelimeleri için renkli vurgulama
- Değişken ve fonksiyon isimleri için özel renklendirme
- Sayısal değerler, operatörler ve metinler için farklı renkler
- Yorum satırları için özel stil
- Gerçek zamanlı güncelleme

### 2. Lexical Analiz
- Token bazlı kod analizi
- Ağaç yapısında token görselleştirme
- Token tipleri:
  - Anahtar Kelimeler (if, else, while, vb.)
  - Tanımlayıcılar (değişken ve fonksiyon isimleri)
  - Sayılar
  - Operatörler
  - Metinler
  - Karakterler
  - Yorumlar

### 3. Sözdizimi Ağacı (Parse Tree)
- Kodun yapısal analizi
- Hiyerarşik ağaç görünümü
- Düğüm tipleri:
  - Program
  - Fonksiyon Tanımları
  - Değişken Tanımları
  - Parametre Listeleri
  - İfadeler (if, while, for)
  - Atama İfadeleri
  - İkili İşlemler
  - Kod Blokları

## Gereksinimler

- C++ derleyici (C11 veya üzeri)
- FLTK (Fast Light Toolkit) kütüphanesi
- CMake (derleme için)

## Kurulum

1. FLTK kütüphanesini yükleyin:
   - Windows: [FLTK İndirme Sayfası](https://www.fltk.org/software.php)
   - Linux: `sudo apt-get install libfltk1.3-dev`
   - macOS: `brew install fltk`

2. Projeyi derleyin:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

## Kullanım

1. Programı başlatın:
   ```bash
   ./ProjePD
   ```

2. Ana pencere üzerinde:
   - Kod düzenleyici bulunur
   - Alt kısımda "Lexical Analiz" ve "Sözdizimi Ağacı" butonları vardır

3. Kod yazma/düzenleme:
   - Kod düzenleyicide C kodu yazın
   - Syntax vurgulama otomatik olarak güncellenir
   - Yorum satırları gri renkte gösterilir

4. Lexical Analiz:
   - "Lexical Analiz" butonuna tıklayın
   - Yeni bir pencerede token'lar ağaç yapısında gösterilir
   - Her token tipi farklı renkte gösterilir

5. Sözdizimi Ağacı:
   - "Sözdizimi Ağacı" butonuna tıklayın
   - Yeni bir pencerede kodun yapısal analizi gösterilir
   - Ağaç yapısını genişletip daraltabilirsiniz

## Örnek Kod

```cpp
// Örnek bir fonksiyon
string hesapla(int x, float y) {
    if (x > 0 && y < 100.0) {
        string sonuc = "Pozitif";
        return sonuc;
    } else {
        while (x != 0) {
            x = x - 1;
            y = y * 2.0;
        }
    }
    return "Bitti";
}
```
## Proje Videosu

-https://youtu.be/sRRPJtxva7s?si=cSyMbg4ywYhfrutt

## Proje Makalesi

-https://medium.com/@yunusayaz51p/sözdimi-renklendiricileri-neden-önemlidir-d02e293979b3

## Proje Yapısı

- `main.cpp`: Ana program ve GUI kodları
- `parse_tree.cpp`: Sözdizimi analizi ve ağaç yapısı
- `CMakeLists.txt`: Derleme yapılandırması
