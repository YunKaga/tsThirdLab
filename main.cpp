#include <iostream>
#include <stdexcept>
#include <initializer_list>

// ==================== ПОСЛЕДОВАТЕЛЬНЫЙ КОНТЕЙНЕР С РЕЗЕРВИРОВАНИЕМ ====================
template <typename T> // шаблон
class posled {
  private:
    T* data_; // сами данные
    size_t size_; // размер заполненных ячеек контейнера
    size_t capacity_; // всего выделено

    void reserve(size_t new_capacity) { // расширение контейнера
      if (new_capacity <= capacity_) return;

      T* new_data = new T[new_capacity];
      try {
        for (size_t i = 0; i < size_; ++i) {
          new_data[i] = std::move(data_[i]);
        }
      }
      catch (...) {
        delete[] new_data;
        throw;
      }

      delete[] data_;
      data_ = new_data;
      capacity_ = new_capacity;
    }

  public:
    // Итератор для последовательного контейнера
    class Iterator {
      private:
        T* ptr_;
      public:
        explicit Iterator(T* ptr) : ptr_(ptr) {} // без неявного преобразования

        T& operator*() { return *ptr_; }
        T* operator->() { return ptr_; }

        Iterator& operator++() {
          ++ptr_;
          return *this;
        }

        Iterator operator++(int) {
          Iterator temp = *this;
          ++ptr_;
          return temp;
        }

        bool operator==(const Iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const Iterator& other) const { return ptr_ != other.ptr_; }
    };

    posled() : data_(nullptr), size_(0), capacity_(0) {}

    explicit posled(size_t size) 
      : size_(size), capacity_(size){
        data_ = new T[capacity_];
        for (size_t i = 0; i < size_; ++i) {
          data_[i] = T();
        }
      }

    posled(std::initializer_list<T> init) 
      : size_(init.size()), capacity_(init.size()){
        data_ = new T[capacity_];
        size_t i = 0;
        for (const auto& item : init) {
          data_[i++] = item;
        }
      }

    // Конструктор перемещения
    posled(posled&& other) noexcept 
      : data_(other.data_), 
      size_(other.size_), 
      capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
      }

    // Оператор присваивания с перемещением
    posled& operator=(posled&& other) noexcept {
      if (this != &other) {
        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
      }
      return *this;
    }

    ~posled() {
      delete[] data_;
    }

    // Копирование запрещено для простоты
    posled(const posled&) = delete;
    posled& operator=(const posled&) = delete;

    T& operator[](size_t index) {
      if (index >= size_) {
        throw std::out_of_range("Index out of range");
      }
      return data_[index];
    }

    const T& operator[](size_t index) const {
      if (index >= size_) {
        throw std::out_of_range("Index out of range");
      }
      return data_[index];
    }

    T& at(size_t index) {
      if (index >= size_) {
        throw std::out_of_range("Index out of range");
      }
      return data_[index];
    }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    void push_back(const T& value) {
      if (size_ >= capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
      }
      data_[size_++] = value;
    }

    void push_back(T&& value) {
      if (size_ >= capacity_) {
        reserve(capacity_ == 0 ? 1 : (capacity_ * 2));
      }
      data_[size_++] = std::move(value);
    }

    void insert(size_t index, const T& value) {
      if (index > size_) {
        throw std::out_of_range("Index out of range");
      }

      if (size_ >= capacity_) {
        reserve(capacity_ == 0 ? 1 : (capacity_ * 2));
      }

      for (size_t i = size_; i > index; --i) {
        data_[i] = std::move(data_[i - 1]);
      }
      data_[index] = value;
      size_++;
    }

    void insert(size_t index, T&& value) {
      if (index > size_) {
        throw std::out_of_range("Index out of range");
      }

      if (size_ >= capacity_) {
        reserve(capacity_ == 0 ? 1 : (capacity_ * 2));
      }

      for (size_t i = size_; i > index; --i) {
        data_[i] = std::move(data_[i - 1]);
      }
      data_[index] = std::move(value);
      size_++;
    }

    void pop_back() {
      if (size_ == 0) {
        throw std::length_error("Container is empty");
      }
      size_--;
    }

    void erase(size_t index) {
      if (index >= size_) {
        throw std::out_of_range("Index out of range");
      }

      for (size_t i = index; i < size_ - 1; ++i) {
        data_[i] = std::move(data_[i + 1]);
      }
      size_--;
    }

    void shrink_to_fit() {
      if (size_ == 0) {
        delete[] data_;
        data_ = nullptr;
        capacity_ = 0;
        return;
      }

      if (size_ < capacity_) {
        T* new_data = new T[size_];
        for (size_t i = 0; i < size_; ++i) {
          new_data[i] = data_[i];
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = size_;
      }
    }

    void clear() {
      size_ = 0;
      delete[] data_;
      data_ = nullptr;
      capacity_ = 0;
    }

    Iterator begin() { return Iterator(data_); }
    Iterator end() { return Iterator(data_ + size_); }

    void print() const {
      for (size_t i = 0; i < size_; ++i) {
        std::cout << data_[i];
        if (i < size_ - 1) std::cout << " ";
      }
      std::cout << std::endl;
    }
};

// ==================== ДВУНАПРАВЛЕННЫЙ СПИСОК ====================
template<typename T>
class DoublyLinkedList {
  private:
    struct Node {
      T data;
      Node* prev;
      Node* next;

      explicit Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}
      explicit Node(T&& value) : data(std::move(value)), prev(nullptr), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;
    size_t size_;

  public:
    class Iterator {
      private:
        Node* current_;
      public:
        explicit Iterator(Node* node) : current_(node) {}

        T& operator*() { return current_->data; }
        T* operator->() { return &current_->data; }

        Iterator& operator++() {
          if (current_) current_ = current_->next;
          return *this;
        }

        Iterator operator++(int) {
          Iterator temp = *this;
          if (current_) current_ = current_->next;
          return temp;
        }

        Iterator& operator--() {
          if (current_) current_ = current_->prev;
          return *this;
        }

        Iterator operator--(int) {
          Iterator temp = *this;
          if (current_) current_ = current_->prev;
          return temp;
        }

        bool operator==(const Iterator& other) const { return current_ == other.current_; }
        bool operator!=(const Iterator& other) const { return current_ != other.current_; }
    };
    // Конструктор
    DoublyLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

    // Конструктор с initializer_list
    DoublyLinkedList(std::initializer_list<T> init) : head_(nullptr), tail_(nullptr), size_(0) {
      for (const auto& item : init) {
        push_back(item);
      }
    }

    // Деструктор
    ~DoublyLinkedList() {
      clear();
    }

    void clear() {
      Node* current = head_;
      while (current) {
        Node* next = current->next;
        delete current;
        current = next;
      }
      head_ = nullptr;
      tail_ = nullptr;
      size_ = 0;
    }

    // Добавление в конец
    void push_back(const T& value) {
      Node* new_node = new Node(value);

      if (size_ == 0) {
        head_ = new_node;
        tail_ = new_node;
      }
      else {
        new_node->prev = tail_;
        tail_->next = new_node;
        tail_ = new_node;
      }
      size_++;
    }

    // Добавление в начало
    void push_front(const T& value) {
      Node* new_node = new Node(value);

      if (size_ == 0) {
        head_ = new_node;
        tail_ = new_node;
      }
      else {
        new_node->next = head_;
        head_->prev = new_node;
        head_ = new_node;
      }
      size_++;
    }

    void insert(size_t index, const T& value) {
      if (index > size_) {
        throw std::out_of_range("Index out of range");
      }

      if (index == 0) {
        push_front(value);
        return;
      }

      if (index == size_) {
        push_back(value);
        return;
      }

      Node* current = get_node(index);
      Node* new_node = new Node(value);

      new_node->prev = current->prev;
      new_node->next = current;
      current->prev->next = new_node;
      current->prev = new_node;

      size_++;
    }

    // Удаление элемента
    void erase(size_t index) {
      if (index >= size_) {
        throw std::out_of_range("Index out of range");
      }

      Node* current = get_node(index);

      if (current->prev) {
        current->prev->next = current->next;
      }
      else {
        head_ = current->next;
      }

      if (current->next) {
        current->next->prev = current->prev;
      }
      else {
        tail_ = current->prev;
      }

      delete current;
      size_--;
    }

    // Доступ по индексу
    T& operator[](size_t index) {
      return get_node(index)->data;
    }

    const T& operator[](size_t index) const {
      return get_node(index)->data;
    }

    // Размер
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    Iterator begin() { return Iterator(head_); }
    Iterator end() { return Iterator(nullptr); }

    // Получение узла по индексу
    Node* get_node(size_t index) const {
      if (index >= size_) {
        throw std::out_of_range("Index out of range");
      }

      // Оптимизация: идем с начала или с конца
      if (index < size_ / 2) {
        Node* current = head_;
        for (size_t i = 0; i < index; ++i) {
          current = current->next;
        }
        return current;
      }
      else {
        Node* current = tail_;
        for (size_t i = size_ - 1; i > index; --i) {
          current = current->prev;
        }
        return current;
      }
    }

    // Вывод содержимого
    void print() const {
      Node* current = head_;
      while (current) {
        std::cout << current->data;
        if (current->next) std::cout << " ";
        current = current->next;
      }
      std::cout << std::endl;
    }
};

// ==================== ОДНОНАПРАВЛЕННЫЙ СПИСОК ====================
template<typename T>
class SinglyLinkedList {
  private:
    struct Node {
      T data;
      Node* next;

      Node(const T& value) : data(value), next(nullptr) {}
      Node(T&& value) : data(std::move(value)), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;
    size_t size_;

  public:
    // Итератор для однонаправленного списка
    class Iterator {
      private:
        Node* current_;
      public:
        explicit Iterator(Node* node) : current_(node) {}

        T& operator*() { return current_->data; }
        T* operator->() { return &current_->data; }

        Iterator& operator++() {
          if (current_) current_ = current_->next;
          return *this;
        }

        Iterator operator++(int) {
          Iterator temp = *this;
          if (current_) current_ = current_->next;
          return temp;
        }

        bool operator==(const Iterator& other) const { return current_ == other.current_; }
        bool operator!=(const Iterator& other) const { return current_ != other.current_; }
    };

    SinglyLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

    SinglyLinkedList(std::initializer_list<T> init) : head_(nullptr), tail_(nullptr), size_(0) {
      for (const auto& item : init) {
        push_back(item);
      }
    }

    // Конструктор перемещения
    SinglyLinkedList(SinglyLinkedList&& other) noexcept 
      : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
      }

    // Оператор присваивания с перемещением
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
      if (this != &other) {
        clear();
        head_ = other.head_;
        tail_ = other.tail_;
        size_ = other.size_;
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
      }
      return *this;
    }

    ~SinglyLinkedList() {
      clear();
    }

    // Копирование запрещено для простоты
    SinglyLinkedList(const SinglyLinkedList&) = delete;
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

    void push_back(const T& value) {
      Node* new_node = new Node(value);
      if (empty()) {
        head_ = tail_ = new_node;
      }
      else {
        tail_->next = new_node;
        tail_ = new_node;
      }
      size_++;
    }

    void push_back(T&& value) {
      Node* new_node = new Node(std::move(value));
      if (empty()) {
        head_ = tail_ = new_node;
      }
      else {
        tail_->next = new_node;
        tail_ = new_node;
      }
      size_++;
    }

    void push_front(const T& value) {
      Node* new_node = new Node(value);
      if (empty()) {
        head_ = tail_ = new_node;
      }
      else {
        new_node->next = head_;
        head_ = new_node;
      }
      size_++;
    }

    void push_front(T&& value) {
      Node* new_node = new Node(std::move(value));
      if (empty()) {
        head_ = tail_ = new_node;
      }
      else {
        new_node->next = head_;
        head_ = new_node;
      }
      size_++;
    }

    void insert(size_t index, const T& value) {
      if (index > size_) {
        throw std::out_of_range("Index out of range");
      }

      if (index == 0) {
        push_front(value);
        return;
      }

      if (index == size_) {
        push_back(value);
        return;
      }

      Node* prev = get_node(index - 1);
      Node* new_node = new Node(value);
      new_node->next = prev->next;
      prev->next = new_node;

      size_++;
    }

    void insert(size_t index, T&& value) {
      if (index > size_) {
        throw std::out_of_range("Index out of range");
      }

      if (index == 0) {
        push_front(std::move(value));
        return;
      }

      if (index == size_) {
        push_back(std::move(value));
        return;
      }

      Node* prev = get_node(index - 1);
      Node* new_node = new Node(std::move(value));
      new_node->next = prev->next;
      prev->next = new_node;

      size_++;
    }

    void erase(size_t index) {
      if (index >= size_) {
        throw std::out_of_range("Index out of range");
      }

      if (index == 0) {
        Node* temp = head_;
        head_ = head_->next;
        if (!head_) tail_ = nullptr;
        delete temp;
      }
      else {
        Node* prev = get_node(index - 1);
        Node* to_delete = prev->next;
        prev->next = to_delete->next;
        if (to_delete == tail_) tail_ = prev;
        delete to_delete;
      }
      size_--;
    }

    T& operator[](size_t index) {
      return get_node(index)->data;
    }

    const T& operator[](size_t index) const {
      return get_node(index)->data;
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() {
      while (head_) {
        Node* temp = head_;
        head_ = head_->next;
        delete temp;
      }
      tail_ = nullptr;
      size_ = 0;
    }

    Iterator begin() { return Iterator(head_); }
    Iterator end() { return Iterator(nullptr); }

    void print() const {
      Node* current = head_;
      while (current) {
        std::cout << current->data;
        if (current->next) std::cout << " ";
        current = current->next;
      }
      std::cout << std::endl;
    }

  private:
    Node* get_node(size_t index) const {
      if (index >= size_) {
        throw std::out_of_range("Index out of range");
      }

      Node* current = head_;
      for (size_t i = 0; i < index; ++i) {
        current = current->next;
      }
      return current;
    }
};

// ==================== ДЕМОНСТРАЦИЯ ВОЗМОЖНОСТЕЙ ====================
void posledovat() {
  std::cout << "=== ПОСЛЕДОВАТЕЛЬНЫЙ КОНТЕЙНЕР ===" << std::endl;
  posled<int> seq;

  // Добавление элементов
  for (int i = 0; i < 10; i++) {
    seq.push_back(i);
  }
  std::cout << "После добавления 0-9: ";
  seq.print();

  std::cout << "Размер: " << seq.size() << std::endl;

  // Удаление элементов
  seq.erase(2); // третий элемент (индекс 2)
  seq.erase(3); // теперь пятый элемент (индекс 3)
  seq.erase(4); // теперь седьмой элемент (индекс 4)
  std::cout << "После удаления 3го, 5го и 7го: ";
  seq.print();

  // Добавление в начало
  seq.insert(0, 10);
  std::cout << "После добавления 10 в начало: ";
  seq.print();

  // Добавление в середину
  seq.insert(4, 20);
  std::cout << "После добавления 20 в середину: ";
  seq.print();

  // Добавление в конец
  seq.push_back(30);
  std::cout << "После добавления 30 в конец: ";
  seq.print();

  // Демонстрация итераторов
  std::cout << "Итерация с помощью итераторов: ";
  for (auto it = seq.begin(); it != seq.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;

  // Демонстрация семантики перемещения
  std::cout << "\n=== СЕМАНТИКА ПЕРЕМЕЩЕНИЯ ===" << std::endl;

  posled<int> original;
  for (int i = 0; i < 5; i++) {
    original.push_back(i * 10);
  }

  posled<int> moved = std::move(original);
  std::cout << "Перемещенный контейнер: ";
  moved.print();
  std::cout << "Исходный контейнер размер: " << original.size() << std::endl;

  // Демонстрация r-value
  int value = 100;
  moved.push_back(std::move(value));
  std::cout << "После добавления r-value: ";
  moved.print();
  std::cout << "Переменная value после перемещения: " << value << std::endl;
}

void DoublyList(){
  std::cout << "\n=== ДВУНАПРАВЛЕННЫЙ СПИСОК ===" << std::endl;
  DoublyLinkedList<int> dll;

  for (int i = 0; i < 10; i++) {
    dll.push_back(i);
  }
  std::cout << "После добавления 0-9: ";
  dll.print();

  std::cout << "Размер: " << dll.size() << std::endl;

  dll.erase(2); dll.erase(3); dll.erase(4);
  std::cout << "После удаления: ";
  dll.print();

  dll.push_front(10);
  std::cout << "После добавления 10 в начало: ";
  dll.print();

  dll.insert(4, 20);
  std::cout << "После добавления 20 в середину: ";
  dll.print();

  dll.push_back(30);
  std::cout << "После добавления 30 в конец: ";
  dll.print();

  std::cout << std::endl;

}
void SinglyList(){
  std::cout << "\n=== ОДНОНАПРАВЛЕННЫЙ СПИСОК ===" << std::endl;
  SinglyLinkedList<int> sll;

  for (int i = 0; i < 10; i++) {
    sll.push_back(i);
  }
  std::cout << "После добавления 0-9: ";
  sll.print();

  std::cout << "Размер: " << sll.size() << std::endl;

  sll.erase(2); sll.erase(3); sll.erase(4);
  std::cout << "После удаления: ";
  sll.print();

  sll.push_front(10);
  std::cout << "После добавления 10 в начало: ";
  sll.print();

  sll.insert(4, 20);
  std::cout << "После добавления 20 в середину: ";
  sll.print();

  sll.push_back(30);
  std::cout << "После добавления 30 в конец: ";
  sll.print();

}

int main() {
  posledovat();
  DoublyList();
  SinglyList();
  return 0;
}
