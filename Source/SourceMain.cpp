/* ���� */
#include "ReverseShell.hpp"

int main(void) {
	try {
		// 8080�� ��Ʈ�� ������ �� ������ �������Ѵ�.
		mawile::ReverseShell* rShell = new mawile::ReverseShell(8080);



		delete rShell;
		return (0);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return (-1);
	}

	return (0);
}