#include <new>

class Row {
	int size;
	int* data;
public:
	Row(const int s = 0): size(s) {
		data = new int[size];
	}
	const int& operator[](const int i) const {
		if (i < 0 || i >= size)
			throw std::out_of_range("Col number is out of ragne");
		return data[i];
	}
	int& operator[](const int i) {
		if (i < 0 || i >= size)
			throw std::out_of_range("Col number is out of ragne");
		return data[i];
	}
	Row& operator=(const Row& r) {
		if (this == &r)
			return *this;
		size = r.size;
		delete[] data;
		data = new int[size];
		for (int i = 0; i < size; i++)
			data[i] = r.data[i];
		return *this;
	}	
	~Row() {
		delete[] data;
	}
};

class Matrix {
	int rows, cols;
	Row* data;
public:
	Matrix(const int r = 0, const int s = 0) :rows(r), cols(s) {
		data = static_cast<Row*>(operator new[] (rows * sizeof(Row)));
		for (int i = 0; i < rows; i++) {
			new (data + i) Row(cols);
		}
	}
	const Matrix& operator*=(const int a) {
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				data[i][j] = data[i][j]*a;
		return *this;
	}
	~Matrix() {
		for (int i = 0; i < rows; i++) {
			data[i].~Row();
		}
		operator delete[] (data);
	}
	const Row& operator[](const int i) const {
		if (i < 0 || i >= rows)
			throw std::out_of_range("Row number is out of ragne");
		return data[i];
	}
	Row& operator[](const int i) {
		if (i < 0 || i >= rows)
			throw std::out_of_range("Row number is out of ragne");
		return data[i];
	}
	bool operator==(const Matrix& m) const {
		if (this == &m)
			return true;
		if (rows != m.rows || cols != m.cols)
			return false;
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < rows; j++)
				if (data[i][j] != m[i][j])
					return false;
		return true;
	}
	bool operator!=(const Matrix& m) const {
		return !(*this == m);
	}
	int getRows() {
		return rows;
	}
	int getColumns() {
		return cols;
	}
};