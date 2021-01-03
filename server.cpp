#include <cstdlib>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <poll.h> 
#include <thread>
#include <unordered_set>
#include <signal.h>
#include <vector>
#include <mutex>
#include <string>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <cmath>

using namespace std;

mutex mtx;
int servFd;

unordered_set<int> clientFds;
void ctrl_c(int);
void sendQuestion(int fd, char * buffer, int count);
uint16_t readPort(char * txt);
void setReuseAddr(int sock);
void recive(int clientFd);

/*
 * structure Quiz
 * 
 * stores the data of quiz:
 *      - AccessCode - code which allows you to join the game
 *      - questionTxt[i] - text of the question[i]
 *      - answer[i] - correct answer for the question[i] 
 *      - capacity - number of questions in this quiz
 * 
 */
struct Quiz{
        int AccessCode;
        string questionTxt[10];
        string answer[10];
        int capacity;
        int authorFd;
    };

/*
 *  lastQuiz stores actual number of quizes
 *  ptr_tab stores pointers to quizes
 */
int lastQuiz = 1;
Quiz * ptr_tab[2];

int main(int argc, char ** argv){
    
    //----------        trail quiz            ---------------//
    Quiz * quiz = (Quiz* ) malloc(2*sizeof(Quiz));
    
    ptr_tab[0] = quiz;
    
    quiz->AccessCode=0;
    quiz->questionTxt[0]="ILe to jest 2+2? \nA. 4 \t B. 5 \t C.3 \t D. 6";
    quiz->answer[0]="a";
    quiz->capacity = 1;
    quiz->authorFd = servFd;
    
    Quiz * quiz1 = (Quiz* ) malloc(1*sizeof(Quiz));
    
    ptr_tab[1] = quiz1;
    
    quiz1->AccessCode=1;
    quiz1->questionTxt[0]="Ile to jest 3+2? \nA. 4 \t B. 5 \t C.3 \t D. 6";
    quiz1->answer[0]="b";
    quiz1->capacity = 1;
    quiz1->authorFd = servFd;
    
	//---------- get and validate port number ---------------//
	if(argc != 2) error(1, 0, "Need 1 arg (port)");
	auto port = readPort(argv[1]);
	
	servFd = socket(AF_INET, SOCK_STREAM, 0);
	if(servFd == -1) 
		error(1, errno, "socket failed");
	
	signal(SIGINT, ctrl_c);
	signal(SIGPIPE, SIG_IGN);
	
	setReuseAddr(servFd);
	
	sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
	int res = bind(servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res) error(1, errno, "bind failed");
	
	res = listen(servFd, 1);
	if(res) 
		error(1, errno, "listen failed");
	
	int num = 0;
	std::thread threads[100];
	int w = 0;
	while(true){
		sockaddr_in clientAddr{0};
		socklen_t clientAddrSize = sizeof(clientAddr);
		
		auto clientFd = accept(servFd, (sockaddr*) &clientAddr, &clientAddrSize);
		if(clientFd == -1) 
			error(1, errno, "accept failed");
		
		clientFds.insert(clientFd);
		
		printf("new connection from: %s:%hu (fd: %d)\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), clientFd);
		

		threads[w++] = std::thread(recive, clientFd);	
	}
	for(int clientFd : clientFds)
		close(clientFd);
	for(int i = 0; i < num; i++)
		threads[i].join();

}

/*
 * function readPort
 * 
 * @param char * txt - pointer to text which stores first argument of function main
 *        
 * function reads the port number as txt and converts it to uint16_t
 * if pointer is null returns the error
 * 
 * return port
 * 
 */
uint16_t readPort(char * txt){
	char * ptr;
	auto port = strtol(txt, &ptr, 10);
	if(*ptr!=0 || port<1 || (port>((1<<16)-1))) 
		error(1,0,"illegal argument %s", txt);
	return port;
}

void setReuseAddr(int sock){
	const int one = 1;
	int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	if(res) 
		error(1,errno, "setsockopt failed");
}
/*
 * function ctrl_c
 * 
 * @param int
 *        
 * function closes server, program and the connections with clients
 *
 */
void ctrl_c(int){
	for(int clientFd : clientFds)
		close(clientFd);
	close(servFd);
	printf("Closing server\n");
    for(Quiz * quiz : ptr_tab){
        free(quiz);
    }
	exit(0);
}

/*
 * function sendQuestion
 * 
 * @param fd - target file descriptor
 *        buffer - message from server
 *        count - individual number of client 
 * 
 * function sends the message to each client which is connected  t othe server
 * 
 */
void sendQuestion(int fd, string buff, int count){
        int res;        
        string q = buff + "\n";
        char temp[q.length()];
        strcpy(temp, q.c_str());
           
        count += (ceil(fd/10) + 3);
        
        decltype(clientFds) bad;

        //for(int clientFd : clientFds){
            //int name = write(clientFd, &fd, sizeof(int));
            
            //mtx.lock();
                res = write(fd, temp, sizeof(temp));           
            //mtx.unlock();
            
        /*   if(res!=count) 
                bad.insert(clientFd);*/
        //}
        for(int clientFd : bad){
            printf("removing %d\n", clientFd);
            clientFds.erase(clientFd);
            close(clientFd);
        }

}


/*
 * function returnNumber
 * 
 * @param buffer - message from client
 *        start - index from which algorithm has to start reading until the special sign - "*"
 * 
 * returns part of buffer - access code of joining game 
 * 
 */
int returnNumber(char *buffer, int start){
    
    string strCode = "";
    int i = start;
    
   while(buffer[i] != '*'){
        strCode += buffer[i];
        i++;
    }

    char temp[strCode.length()];
    strcpy(temp, strCode.c_str());

    int number = atoi(temp);     
    return number; 
}

/*
 * function returnSepLoc
 * 
 * @param buffer - message from client
 * 
 * returns location of the first separator - "*" in the message from client
 * 
 */
int returnSepLoc(char *buffer){
    
    int sep, i=0;
    
    while(buffer[i] != '*'){ 
            i++;
        }
        
        sep = i;
     
    return sep; 
}

/*
 * function returnQuestion
 * 
 * @param buffer - message from client
 *        start - index from which algorithm has to start reading until the special sign - "*"
 * 
 * returns part of buffer - string which is a :
 *      - text of the question or
 *      - given answer for question 
 * 
 */
string returnQuestion(char *buffer, int start){
    
    string strCode = "";
    int i = start;
    
    while(buffer[i] != '*') {
            strCode += buffer[i];
            i++;
    }

    char temp[strCode.length()];
    strcpy(temp, strCode.c_str());

    return temp; 
}

/*
 * function recive
 * 
 * @param clientFd - client's file descriptor
 * 
 * reads the message from the client and depending on the sign readed from the message 
 * either joins the quiz or creates a quiz
 * 
 * clients_answers  - number of correct answers given by client
 * buffer           - message from client
 * acces            - access code to quiz
 * index            - index of the joining quiz  
 * 
 * questionNew      - the content of new question 
 * answerNew        - the correct answer of new question
 * accesNew         - access code of new quiz
 * capNew           - number of questions in new quiz
 */
void recive(int clientFd)
{
	while(1)
	{
		char buffer[255];
		int count = read(clientFd, buffer, 255);
        
		if(count < 1) {
			printf("removing %d\n", clientFd);
			clientFds.erase(clientFd);
			close(clientFd);
			break;
        }
		
		//if join button was clicked
		char sign = buffer[0];
        
            if(sign == '&'){
            
                int acces = returnNumber(buffer, 1);
                int clients_answers = 0;
                int index;
                
                for(int i=0; i<lastQuiz+1; i++){ 
                    if(acces == ptr_tab[i]->AccessCode){
                        index = i;
                    }
                    else{
                      continue;   
                    }
                
                        if(clientFd == ptr_tab[index]->authorFd){
                            string message = "!*";
                            sendQuestion(clientFd, message, count);
                                
                            message = "lub nie możesz wziąć udziału we własnym quizie.";
                            sendQuestion(clientFd, message, count);
                                
                        } 
                        else{

    //---------- send question and read the answer -----------------//            
                        for(int i=0; i< ptr_tab[index]->capacity; i++){
                            sendQuestion(clientFd, ptr_tab[index]->questionTxt[i], count);

    //------------------ wait for the answer -----------------------//
                            //mtx.lock();
                            while(buffer[0] != '?'){
                                count = read(clientFd, buffer, 255); 
                                continue;
                            }
                           // mtx.unlock();
                            string answerUser = returnQuestion(buffer, 1);
                        
    //----------- compare the answer with the correct one -----------//
                            if(ptr_tab[index]->answer[i] == answerUser){
                                    
                            clients_answers++;
                            }
                                    
                        }
                        
                        string txt = "!#";
                        sendQuestion(clientFd, txt, count);
                        
                        //send the result
                        string finalResult = "Twój wynik: "+to_string(clients_answers);
                        sendQuestion(clientFd, finalResult, count);
                    }
                }
                
            }
    //----------- if create a new quiz button was clicked -----------//
            else if(sign == '#'){
               
                //count = read(clientFd, buffer, 255); 
                //ptr_tab = (Quiz *) malloc(sizeof(Quiz));
                Quiz * quiz1 = (Quiz *) malloc(sizeof(Quiz));
                ptr_tab[lastQuiz+1] = new Quiz;
                ptr_tab[lastQuiz+1] = quiz1;
                
                ptr_tab[lastQuiz+1]->authorFd = clientFd;
 
                int accesNew = returnNumber(buffer, 1);
                ptr_tab[lastQuiz+1]->AccessCode = accesNew;
                
                int firstSep = returnSepLoc(buffer);
                int capNew = returnNumber(buffer, firstSep+1);
                ptr_tab[lastQuiz+1]->capacity = capNew;

    //------------- read and save questions and answers -------------//
                
                for(int i=0; i<capNew; i++){
                   
                    count = read(clientFd, buffer, 255);
                    string questionNew = returnQuestion(buffer,0);
                    
                    ptr_tab[lastQuiz+1]->questionTxt[i] = questionNew.c_str(); 

                    int firstSep = returnSepLoc(buffer);
                    string answerNew = returnQuestion(buffer, firstSep+1);
                    ptr_tab[lastQuiz+1]->answer[i] = answerNew.c_str();
                }
                 lastQuiz+=1;
            }
		}
	}

