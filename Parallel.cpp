unsigned int& Parallel::operator[](int index){
    return arr[index];
}

const unsigned int& operator[](int) const{
    return arr[index];
}
unsigned int& operator()(int){
    return arr[index];
}
const unsigned int& operator()(int) const{
    return arr[index];
}