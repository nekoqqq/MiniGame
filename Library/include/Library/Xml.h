#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <cctype>
#include <cassert>
#include <iostream>
#include <sstream>
#include <stack>
using std::string;
using std::vector;
using std::ifstream;
using std::unordered_map;
using std::istringstream;


class Element {
public:
	Element(const string& name, int level) :tag_name(name), level(level)
	{
		std::cout << "creating Element" << " ";
		std::cout << *this;
	}
	~Element()
	{
		for (auto& child : children) {
			std::cout << "deleting Element " << child->tag_name << std::endl;
			delete child;
			child = nullptr;
		}

	}
	// TODO 这个函数直接返回attr[t]一直报错加了const修饰符也没用,下面的代码是可以运行的
	const string getAttr(const string &t)const {
		auto it = attributes.find(t);
		if (it != attributes.end())
			return it->second;
		return "";
	}


	const vector<Element*>& getChildren()const
	{
		return children;
	}
	const string& getTagName()const {
		return tag_name;
	}
	void addChild(Element* child)
	{
		children.push_back(child);
	}
	void addAttr(const string& attr_kv)
	{
		const size_t i = attr_kv.find('=');
		const size_t n = attr_kv.size();
		// 忽略"
		if (i != string::npos)
			attributes[attr_kv.substr(0, i)] = attr_kv.substr(i + 2, n - i - 3);
	}
	void handle() {
		if (children.empty())
			return;
		for (unsigned i = 0; i < children.size(); i++) {
			children[i]->handle();
		}
	}
	friend std::ostream& operator<<(std::ostream& os, const Element& e)
	{
		string indent(e.level * 2, ' ');
		os << indent << "name: " << e.tag_name << " " << "level " << e.level << ", attributes: ";
		for (auto& kv : e.attributes)
			os << "<" << kv.first << "," << kv.second << ">" << ", ";
		return os;
	}
	void print()
	{
		std::cout << *this << std::endl;
		for (const auto& child : children)
			child->print();
	}

	// TODO将来可以把attribute单独放出来
	template<typename T>
	static vector<T> converToArray(const string& s) {
		std::istringstream iss(s);
		vector<T> res;
		string tmp;
		while (std::getline(iss, tmp))
		{
			istringstream iss2(tmp);
			T t;
			while (iss2 >> t) {
				res.push_back(t);
				iss2.ignore(tmp.size(), ',');
			}
		}
		return res;
	}

	static unsigned convertHexToUnsigned(const string& s) {
		std::istringstream iss(s);
		unsigned res;
		iss >> std::hex >> res;
		return res;
	}
private:
	int level; // 在树中的层次 0 为根节点
	string tag_name;
	unordered_map<string, string> attributes;
	vector<Element*> children;
};

class XMLParser {
public:
	enum State {
		INITIAL, // 初始状态
		LEFT_NAME_START, // 发现 <
		LEFT_NAME_END, // 发现 >
		LEFT_ATTR_START, // 发现属性
		LEFT_ATTR_END, // 属性结束
	};
	XMLParser(const char* file_name) {
		readFile(file_name);
		std::cout << "creating XML" << std::endl;
	}
	~XMLParser() {
		std::cout << "deleting XML" << std::endl;
		delete root;
		std::cout << "deleting root" << std::endl;
		root = nullptr;
	}
	void readFile(const char* file_name) {
		// 受限版本XML文件实现
		// 不支持<name/>格式，不支持<name> value </name>, name和<>之间无空格，<name t=s>>/name> t、=、s之间无空格
		ifstream fin(file_name);
		if (!fin.is_open())
		{
			std::cerr << "Error: Could not open file: " << file_name << std::endl;
			exit(1);
		}
		std::stringstream ss;
		ss << fin.rdbuf();
		buffer = ss.str();

		int i = 0;
		int n = buffer.size();
		State current_state = INITIAL;
		std::stack<Element*> st;
		while (i < n) {
			switch (current_state)
			{
			case INITIAL:
				while (i < n && isspace(buffer[i]))
					i++;
				current_state = LEFT_NAME_START;
				break;
			case LEFT_NAME_START:
			{
				i++; // skip <
				// 跳过注释
				if (buffer[i] == '!')
				{
					while (i < n && buffer[i] != '>')
						i++;
					i++;
					current_state = LEFT_ATTR_END;
					break;
				}
				string tmp;
				while (i < n && buffer[i] != '>' && !isspace(buffer[i]))
				{
					tmp += buffer[i];
					i++;
				}
				Element* p = new Element(tmp, st.size());
				if (st.empty())
				{
					root = p;
				}
				else
				{
					st.top()->addChild(p);
				}
				st.push(p);
				current_state = LEFT_NAME_END;
			}
			break;
			case LEFT_NAME_END:
				if (buffer[i] == '>')
				{
					i++;
					while (i < n && isspace(buffer[i]))
						i++;
					current_state = LEFT_ATTR_END;
				}
				else
				{
					while (i < n && isspace(buffer[i]))
						i++;
					current_state = LEFT_ATTR_START;
				}
				break;
			case LEFT_ATTR_START:
			{
				string tmp;
				int quota_cnt = 0;
				while (i < n && quota_cnt != 2 && buffer[i] != '>') // 24和"24"都要支持
				{
					if (buffer[i] == '"')
						quota_cnt++;
					tmp += buffer[i++];
				}
				st.top()->addAttr(tmp);
				if (buffer[i] == '>')
				{
					i++;
					current_state = LEFT_ATTR_END;
				}
				else
				{
					while (i < n && isspace(buffer[i]))
						i++;
					current_state = LEFT_ATTR_START;
				}
			}
			break;
			case LEFT_ATTR_END:
				while (i < n && isspace(buffer[i]))
					i++;
				if (buffer[i] == '<' && buffer[i + 1] == '/')
				{
					while (i < n && buffer[i] != '>')
						i++;
					i++; // skip '>'
					current_state = LEFT_ATTR_END;
					if (!st.empty())
						st.pop();
				}
				else
				{
					current_state = LEFT_NAME_START;
				}
				break;
			}
		}
	}
	void print() const
	{
		std::cout << *root << std::endl;
		for (const auto child : root->getChildren())
			child->print();
	}
	const Element* getRoot()const {
		return root;
	}

private:
	Element* root;
	string buffer;
};