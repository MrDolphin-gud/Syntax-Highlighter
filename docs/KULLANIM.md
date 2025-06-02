# Kullanım Kılavuzu

Bu kılavuz, Sözdizimi Ağacı Görselleştirici'nin nasıl kullanılacağını adım adım açıklar.

## Başlangıç

1. Programı başlatın
2. Ana pencere açıldığında, sol tarafta bir metin düzenleyici göreceksiniz
3. Sağ üst köşedeki "Sözdizimi Ağacı" butonuna tıklayın

## Temel Kullanım

### Örnek 1: Basit Bir Fonksiyon

Aşağıdaki kodu metin düzenleyiciye girin:

```cpp
int topla(int a, int b) {
    return a + b;
}
```

Sözdizimi ağacı şu şekilde görünecektir:

```
Program
└── Fonksiyon Tanımı: topla
    ├── Veri Tipi: int
    ├── Parametre Listesi
    │   ├── Parametre
    │   │   ├── Veri Tipi: int
    │   │   └── Tanımlayıcı: a
    │   └── Parametre
    │       ├── Veri Tipi: int
    │       └── Tanımlayıcı: b
    └── Kod Bloğu
        └── Return İfadesi
            └── İkili İşlem: +
                ├── Tanımlayıcı: a
                └── Tanımlayıcı: b
```

### Örnek 2: Koşullu İfadeler

```cpp
void kontrol(int x) {
    if (x > 0) {
        return 1;
    } else {
        return -1;
    }
}
```

Ağaç yapısı:

```
Program
└── Fonksiyon Tanımı: kontrol
    ├── Veri Tipi: void
    ├── Parametre Listesi
    │   └── Parametre
    │       ├── Veri Tipi: int
    │       └── Tanımlayıcı: x
    └── Kod Bloğu
        └── If İfadesi
            ├── İkili İşlem: >
            │   ├── Tanımlayıcı: x
            │   └── Sabit Değer: 0
            ├── Kod Bloğu
            │   └── Return İfadesi
            │       └── Sabit Değer: 1
            └── Kod Bloğu
                └── Return İfadesi
                    └── Sabit Değer: -1
```

### Örnek 3: Döngüler

```cpp
int faktoriyel(int n) {
    int sonuc = 1;
    for (int i = 1; i <= n; i++) {
        sonuc = sonuc * i;
    }
    return sonuc;
}
```

Ağaç yapısı:

```
Program
└── Fonksiyon Tanımı: faktoriyel
    ├── Veri Tipi: int
    ├── Parametre Listesi
    │   └── Parametre
    │       ├── Veri Tipi: int
    │       └── Tanımlayıcı: n
    └── Kod Bloğu
        ├── Değişken Tanımı
        │   ├── Veri Tipi: int
        │   ├── Tanımlayıcı: sonuc
        │   └── Sabit Değer: 1
        ├── For Döngüsü
        │   ├── Değişken Tanımı
        │   │   ├── Veri Tipi: int
        │   │   ├── Tanımlayıcı: i
        │   │   └── Sabit Değer: 1
        │   ├── İkili İşlem: <=
        │   │   ├── Tanımlayıcı: i
        │   │   └── Tanımlayıcı: n
        │   ├── İkili İşlem: ++
        │   │   └── Tanımlayıcı: i
        │   └── Kod Bloğu
        │       └── Atama İfadesi
        │           ├── Tanımlayıcı: sonuc
        │           └── İkili İşlem: *
        │               ├── Tanımlayıcı: sonuc
        │               └── Tanımlayıcı: i
        └── Return İfadesi
            └── Tanımlayıcı: sonuc
```

## İpuçları ve Püf Noktaları

1. **Gerçek Zamanlı Güncelleme**
   - Kod değişikliklerinde ağaç otomatik olarak güncellenir
   - Büyük dosyalarda güncelleme biraz zaman alabilir

2. **Ağaç Görünümü**
   - Düğümleri genişletmek/daraltmak için ok işaretlerine tıklayın
   - Ağacı kaydırmak için fare tekerleğini kullanın
   - Tüm düğümleri genişletmek için Ctrl+A tuşlarına basın

3. **Hata Durumları**
   - Sözdizimi hataları kırmızı renkle gösterilir
   - Hatalı kısımlar ağaçta "Bilinmeyen" düğümü olarak görünür
   - Hataları düzelttikçe ağaç otomatik olarak güncellenir

4. **Performans**
   - Çok büyük dosyalarda (1000+ satır) performans düşebilir
   - Gereksiz yere büyük dosyaları açmaktan kaçının
   - Ağaç görünümünü geçici olarak kapatarak performansı artırabilirsiniz

## Sık Karşılaşılan Sorunlar

1. **Ağaç Güncellenmiyor**
   - Metin düzenleyicide değişiklik yaptığınızdan emin olun
   - Programı yeniden başlatmayı deneyin
   - Kaynak kodun geçerli C++ sözdizimine uygun olduğunu kontrol edin

2. **Düğümler Görünmüyor**
   - Ağaç görünümünün açık olduğundan emin olun
   - Düğümleri genişletmek için ok işaretlerine tıklayın
   - Kaynak kodun doğru girildiğini kontrol edin

3. **Performans Sorunları**
   - Büyük dosyaları daha küçük parçalara bölün
   - Ağaç görünümünü geçici olarak kapatın
   - Gereksiz kod bloklarını kaldırın

## İleri Seviye Kullanım

### Özel Düğüm Tipleri

Program, aşağıdaki özel düğüm tiplerini destekler:

- `PROGRAM`: Programın kök düğümü
- `FUNCTION_DEF`: Fonksiyon tanımları
- `VARIABLE_DECL`: Değişken tanımlamaları
- `PARAM_LIST`: Parametre listeleri
- `STATEMENT`: Genel ifadeler
- `EXPRESSION`: İfadeler ve operatörler

### Ağaç Yapısını Anlama

Her düğüm şu bilgileri içerir:
- Düğüm tipi (örn. Fonksiyon Tanımı, If İfadesi)
- Değer (varsa)
- Alt düğümler

Ağaç yapısını anlamak için:
1. Kök düğümden başlayın
2. Alt düğümleri inceleyin
3. Düğüm tiplerini ve değerlerini analiz edin
4. Kod yapısını ağaç yapısıyla eşleştirin 