# KamataEngine
2022前期
![Screen Shot 2022-03-01 at 16 51 58](https://user-images.githubusercontent.com/50472340/156130215-737758dc-82b1-4f40-892e-ee7517b6f88f.png)

## Novice/Adapterについて

1年次に利用する教材です。KamataEngine本体の上に簡便に利用できる関数を用意してラップしたものです。  
学生の利便性などから、Cドライブ直下にKamataEngineがある前提でプロジェクトが作られています。  
2年次以降、Novice/Adapterの2ディレクトリは利用しません。  
Cドライブ直下に移動して、`git clone https://github.com/sfujisawa2019/KamataEngine.git`でクローンできます。
### 構成
```
C
|--KamataEngine
|   |-- ...
|
|-- ...（ユーザーディレクトリなど）
    |-- Novice（KamataEngineからコピーしてくる）
    |-- Project1（課題ごとに学生がNoviceをコピーして作成）
    |-- ...
```

### 想定するフロー

1. googleドライブから（教員はgitから）、KamataEngineをC直下に配置する
2. 作業用ディレクトリ（どこでも良い）にNoviceをコピーする
3. コピーしたNoviceをテンプレート登録する（AL1で行う）
4. テンプレートを元に授業の課題ごとにプロジェクトを量産
5. main.cppに課題を記述してclass roomに提出
