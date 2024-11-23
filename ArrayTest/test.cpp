#include "pch.h"

#include <vector>

#include "../DynamicArray/src/Array.h"

class MovableClass {
public:
	MovableClass(int val) : value(val) {}
	MovableClass(const MovableClass& other) : value(other.value) {}
	MovableClass& operator=(const MovableClass& other) { value = other.value; return *this; }
	MovableClass(MovableClass&& other) : value(other.value) { other.value = 0; }
	MovableClass& operator=(MovableClass&& other) { value = other.value; other.value = 0; return *this; }
	~MovableClass() = default;

	friend bool operator==(const MovableClass& a, const MovableClass& b) { return a.value == b.value; }
	friend bool operator!=(const MovableClass& a, const MovableClass& b) { return a.value != b.value; }

private:
	int value;
};

class UnmovableClass
{
public:
	UnmovableClass(int v) : value(v) {};
	UnmovableClass(const UnmovableClass& other) : value(other.value) {};
	UnmovableClass& operator=(const UnmovableClass& other) { value = other.value; return *this; };
	UnmovableClass(UnmovableClass&& other) = delete;
	UnmovableClass& operator=(UnmovableClass&& other) = delete;
	~UnmovableClass() {};

	friend bool operator==(const UnmovableClass& a, const UnmovableClass& b) { return a.value == b.value; }
	friend bool operator!=(const UnmovableClass& a, const UnmovableClass& b) { return a.value != b.value; }

private:
	int value;
};

TEST(ArrayTests, IntegerPositiveCheck) {
	myStl::Array<int> a = { 1, 2, 3 };
	myStl::Array<int> b = { 1, 2, 3 };
	EXPECT_EQ(a, b);
}

TEST(ArrayTests, IntegerNegativeCheck) {
	myStl::Array<int> a = { 1, 2, 3 };
	myStl::Array<int> b = { 1, 2, 2 };
	EXPECT_FALSE(a == b);
}

TEST(ArrayTests, IntegerInsertCheck) {
	myStl::Array<int> a = { 1, 2 };
	myStl::Array<int> b = { 1, 2, 3 };
	a.insert(3);

	EXPECT_EQ(a, b);
}

TEST(ArrayTests, IntegerRemoveCheck) {
	myStl::Array<int> a = { 1, 2 };
	myStl::Array<int> b = { 1, 2, 3 };
	b.remove(2);

	EXPECT_EQ(a, b);
}

TEST(ArrayTests, IntegerIterationCheck) {
	myStl::Array<int> a = { 1, 2, 3, 4 };
	std::vector<int> tmp(4);
	std::vector<int> control = { 1, 2, 3, 4 };
	int i = 0;
	for (auto it = a.constIterator(); it.hasNext(); it.next())
		tmp[i++] = it.get();

	EXPECT_EQ(tmp, control);
}

TEST(ArrayTests, IntegerReverseIterationCheck) {
	myStl::Array<int> a = { 1, 2, 3, 4 };
	std::vector<int> tmp(4);
	std::vector<int> control = { 4, 3, 2, 1 };
	int i = 0;
	for (auto it = a.reverseIterator(); it.hasNext(); it.next())
		tmp[i++] = it.get();

	EXPECT_EQ(tmp, control);
}

TEST(ArrayTests, IntegerUltimateCheck) {
	myStl::Array<int> a;
	for (int i = 1; i < 5; i++)
		a.insert(i);
	for (auto it = a.iterator(); it.hasNext(); it.next())
		it.get() *= 2;
	

	EXPECT_EQ(a, myStl::Array<int>({2, 4, 6, 8}));
}


TEST(ArrayTests, IntegerRangeForCheck) {
	myStl::Array<int> a = { 1, 2, 3 };
	for (auto& it : a)
		it *= 2;
	EXPECT_EQ(a, myStl::Array<int>({2, 4, 6}));
}

// Проверить что работает с кастомным классом
TEST(ArrayTests, MovableInsertCheck) {
	myStl::Array<MovableClass> a = { 1, 2 };
	myStl::Array<MovableClass> b = { 1, 2, 3 };
	a.insert(3);

	EXPECT_EQ(a, b);
}

TEST(ArrayTests, MoveCheck)
{
	myStl::Array<MovableClass> a = { 1, 2, 3 };
	myStl::Array<MovableClass> move = { std::move(a) };
	EXPECT_EQ(move, myStl::Array<MovableClass>({ 1, 2, 3 }));
	EXPECT_EQ(a, myStl::Array<MovableClass>());
}

TEST(ComplexTest, UnmovableClassCheck)
{
	myStl::Array<UnmovableClass> a = { 1, 2 };
	myStl::Array<UnmovableClass> b = { 1, 3 };
	a.insert(3);
	a.remove(1);

	EXPECT_EQ(a, b);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}