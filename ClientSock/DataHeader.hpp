#ifndef H_DATAHEADER
#define H_DATAHEADER

enum CMD {
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER,
	CMD_ERROR
};

struct DataHeader {
	short data_length;
	CMD cmd;
};

struct Login :public DataHeader {
	Login() {
		data_length = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char username[32];
	char password[32];
};

struct Logout :public DataHeader {
	Logout() {
		data_length = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char username[32];
};

struct LoginResult :public DataHeader {
	LoginResult() {
		data_length = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	short result;
};

struct LogoutResult :public DataHeader {
	LogoutResult() {
		data_length = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	short result;
};

struct NewUser :public DataHeader {
	NewUser() {
		data_length = sizeof(NewUser);
		cmd = CMD_NEW_USER;
		result = 0;//1新用户登录，0用户退出
		sock = INVALID_SOCKET;
	}
	short result;
	SOCKET sock;
};

#endif