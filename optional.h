#include <stdexcept>
#include <utility>

// Исключение этого типа должно генерироватся при обращении к пустому optional
class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() = default;
    Optional(const T& value);
    Optional(T&& value);
    Optional(const Optional& other);
    Optional(Optional&& other);

    Optional& operator=(const T& value);
    Optional& operator=(T&& rhs);
    Optional& operator=(const Optional& rhs);
    Optional& operator=(Optional&& rhs);

    ~Optional();

    bool HasValue() const;
    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста
    T& operator*()&;
    const T& operator*() const&;
    T* operator->();
    const T* operator->() const;
    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value()&;
    const T& Value() const&;
    void Reset();

    template <typename... VT>
    void Emplace(VT&&... vt);

    T&& operator*() &&;
    T&& Value() &&;

private:
    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;
    T* ptr_ = nullptr;
};

// template <typename T>
// Optional<T>::Optional() {
//     ptr_ = new (&data_[0]) T({});
// }

template <typename T>
Optional<T>::Optional(const T& value) {
    ptr_ = new (&data_[0]) T(value);
    is_initialized_ = true;
}

template <typename T>
Optional<T>::Optional(T&& value) {
    ptr_ = new (&data_[0]) T(std::move(value));
    is_initialized_ = true;
}

template <typename T>
Optional<T>::Optional(const Optional& other) {
    if (other.HasValue()) {
        ptr_ = new (&data_[0]) T(other.Value());
        is_initialized_ = true;
    } 
}

template <typename T>
Optional<T>::Optional(Optional&& other) {
    if (other.HasValue()) {
        ptr_ = new (&data_[0]) T(std::move(other.Value()));
        is_initialized_ = true;
    }
}
template <typename T>
Optional<T>& Optional<T>::operator=(const T& value){
    if  (is_initialized_) {
        *ptr_= (value);
    }
    else {
        ptr_ = new (&data_[0]) T(value);
        is_initialized_ = true;
    }
    return *this;
}
template <typename T>
Optional<T>& Optional<T>::operator=(T&& rhs){
    if  (is_initialized_) {
        *ptr_= (std::move(rhs));
    }
    else {
        ptr_ = new (&data_[0]) T(std::move(rhs));
        is_initialized_ = true;
    }
    return *this;
}
template <typename T>
Optional<T>& Optional<T>::operator=(const Optional& rhs){
    if (rhs.HasValue()) {
        if  (is_initialized_) {
            
            *ptr_= (rhs.Value());
           
        }
        else {
            
            ptr_ = new (&data_[0]) T(rhs.Value());
            is_initialized_ = true;
            
        }
    }
    else {
        ptr_->~T();
        ptr_ = nullptr;
        is_initialized_ = rhs.is_initialized_;
        
    }
        
    return *this;
}
template <typename T>
Optional<T>& Optional<T>::operator=(Optional&& rhs){
    if (rhs.HasValue()) {
        if  (is_initialized_) {
            *ptr_= (std::move(rhs.Value()));
        }
        else {
            ptr_ = new (&data_[0]) T(std::move(rhs.Value()));
            is_initialized_ = true;
        }
    }
    else {
        // ptr_->~T();
        this->Reset();}
    return *this;
}

template <typename T>
Optional<T>::~Optional() {
    Reset();
}

template <typename T>
bool Optional<T>::HasValue() const {
    return ptr_ != nullptr;
}

// Операторы * и -> не должны делать никаких проверок на пустоту Optional.
// Эти проверки остаются на совести программиста
template <typename T>
T& Optional<T>::operator*()& {
    return *ptr_;
}

template <typename T>
const T& Optional<T>::operator*() const&{
    return *ptr_;
}

template <typename T>
T&& Optional<T>::operator*() && {
    return std::move(*ptr_);
}


template <typename T>
T* Optional<T>::operator->(){
    return ptr_;
}
template <typename T>
const T* Optional<T>::operator->() const {
/// проверить!!!
    return ptr_;
}

template <typename T>
void Optional<T>::Reset() {
    
    if (is_initialized_) {
        ptr_->~T();
        ptr_ = nullptr;
    }
    is_initialized_ = false;
}

// Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
template <typename T>
T& Optional<T>::Value()&{
    if (ptr_ != nullptr && is_initialized_)  {return *ptr_;}
    else {
        throw BadOptionalAccess();
    }
}

template <typename T>
T&& Optional<T>::Value() && {
    if (ptr_ != nullptr && is_initialized_)  {return std::move(*ptr_);}
    else {
        throw BadOptionalAccess();
    }
}

template <typename T>
const T& Optional<T>::Value() const& {
    if (ptr_ != nullptr && is_initialized_)  {return *ptr_;}
    else {
        throw BadOptionalAccess();
    }
}

template <typename T>
template <typename... VT>
void Optional<T>::Emplace(VT&&... vt) {
    if (HasValue()) { Reset();}
    
        //*this=std::move(Optional<T>({(std::forward<VT>(vt))...}));
        ptr_ = new(&data_[0]) T(std::forward<VT>(vt)...);
        is_initialized_ = true; 
}

