class Square {
 public:
    explicit Square(int size) : size_(size) {}

    // デフォルトコンストラクタ以外のコンストラクタが存在するので
    // デフォルトコンストラクタの暗黙的な定義は行われない。
    // デフォルトコンストラクタを使用するために default 指定で明示的に定義
    Square() = default;

    int Area() const {
        return size_ * size_;
    }

 private:
    int size_;
};

int main() {
    Square s1;
    Square s2(10);

    return 0;
}
