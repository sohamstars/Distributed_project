#include <stdio.h>
#include <iostream>
#include <string>
#include "sha256.h"
int main()
{
	std::string s="www.slack.com1492630700";
	std::cout<<"sha "<<sha256(s)<<std::endl;
}
