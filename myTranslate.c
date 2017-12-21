#include <stdio.h>
#include <python2.7/Python.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#define MAX_BUF 1024

pthread_t tid1; // 시그널에 할당하기 위해 전역변수로 tid를 설정하였습니다.

void SigHandler(int signo){ // 시그널 함수입니다. 번역 도중 종료가 되면 스레드 동작을 멈추고 번역한곳 까지 저장하여  정상 종료될 수 있게 합니다.
	if(signo == SIGINT){
		Py_Initialize(); // 파이썬 실행을 위해 매소드 호출
		if(Py_IsInitialized()){ // 정상 호출이 되었다면 아래와 같이 작업합니다.
			PyRun_SimpleString("print('Translate Stop')"); // 번역이 멈췄음을 출력합니다.
               		PyRun_SimpleString("with open('.tempTanslate', 'w') as tt:\n    tt.writelines('\\n'.join(tlStr))"); // 번역한 곳까지 임시 저장합니다. 저장파일명은 .tempTanslate로 합니다.
			PyRun_SimpleString("print('SAVE UNTIL INPUT SIGINT')"); // 시그널이 발생했음을 출력합니다.
		}
		pthread_cancel(tid1); // 스레드를 종료합니다.
	}
	exit(0);
}

void PyTranslate(FILE* src){  // 스레드를 위한 함수입니다. 파라미터로 번역할 input파일의 정보를 받습니다.
   char buf[MAX_BUF];

   Py_Initialize(); // 번역을 위한 파이썬 코드를 실행합니다. 구글 번역 API가 C언어를 지원하지 않아 파이썬을 이용하였습니다.
        if(Py_IsInitialized()) { // 정상적으로 파이썬이 불러왔을 경우 파이썬 코드를 실행합니다.
                PyRun_SimpleString("print('I am Python2.7!')"); // 먼저 파이썬 2.7이 정상적으로 열였음을 알립니다.
                PyRun_SimpleString("from google.cloud import translate"); // 번역위한 모듈을 호출합니다.
                PyRun_SimpleString("translate_client = translate.Client()");
                PyRun_SimpleString("target = 'jpn'"); // 1단계로는 영어를 일본어로 번역합니다.
                PyRun_SimpleString("target2 = 'ko'"); // 2단계로는 일본어를 한글로 번역합니다. 영어에서 한글로 바로 번역하는것 보다 일본어를 거치는것이 보다 번역이 정확하기 때문입니다.
                PyRun_SimpleString("tlStr = []"); // 번역한 line별로 리스트에 저장하기 위한 변수입니다.
                PyRun_SimpleString("print 'Translation:'"); // 번역이 시작되었음을 알립니다.
		PyRun_SimpleString("print('Translate to Japanese End')");
		PyRun_SimpleString("print('Translate to Korea End')");
                while(fgets(buf, MAX_BUF, src) != NULL) { // line별로 번역하기 때문에 while문을 통해 반복하게 하였습니다. line별로 buf로 저장합니다.
                        buf[strlen(buf) - 1] = '\0'; // 개행문자를 제거하기 위한 코드입니다.
                        char mystr[MAX_BUF+10]; // 구글 API를 사용하기 위한 template을 만들 배열입니다.
                        sprintf(mystr, "text = u'%s'", buf);
                        PyRun_SimpleString(mystr); // 번역할 string(line)을 text 변수에 저장합니다.
                        PyRun_SimpleString("translation = translate_client.translate(text, target_language=target)"); // API 호출 부분입니다. 영어에서 일본어로 번역을 요청합니다.
                        PyRun_SimpleString("text2 = translation['translatedText']"); // 번역된 문장을 text2에 저장합니다.
                        PyRun_SimpleString("translation = translate_client.translate(text2, target_language=target2)"); // 일본어로 번역된 문장을 한글로 번역을 요청합니다.
                        PyRun_SimpleString("print(u'{}'.format(translation['translatedText']))"); // 한글로 번역된 문장을 stdout으로 출력합니다.
                        PyRun_SimpleString("tlStr += ['{}'.format(translation['translatedText'].encode('utf-8'))]"); // 한글로 번역된 문장을 tlStr 배열에 추가합니다.
                }
		PyRun_SimpleString("print('Translate finish')"); // 모든 문장이 번역이 되었음을 알립니다.
                PyRun_SimpleString("with open('.tempTanslate', 'w') as tt:\n    tt.writelines('\\n'.join(tlStr))"); // 번역된 문장을 .tempTanslate 파일에 임시 저장합니다.

        }
        Py_Finalize(); // 모든 파이썬 코드가 끝났음을 명시해줍니다.

}


int main(int argc, char *argv[])
{
	signal(SIGINT, SigHandler); // 번역도중 종료되는 사항을 위한 singal 함수를 등록합니다.
        FILE *src;

        if(argc != 2 && argc != 3) { // 2개의 파라미터가 온다면 번역하여 stdout만 출력하고, 3개가 온다면 stdout 출력과 파일로 저장하기 위함입니다.
                fprintf(stderr, "Usage: %s source destination\n", argv[0]);
                exit(1);
        }
        if ((src = fopen(argv[1], "rt")) == NULL)  { // 2번째 파라미터로 온 변수는 input으로 받습니다.
                perror("fopen");
                exit(1);
        }

        if(pthread_create(&tid1, NULL, (void*)PyTranslate, (void*)src)<0){ // input으로 받은 파일을 스레드를 통해 번역을 요청합니다.
                perror("pthread_create");
                exit(1);
        }

        if(pthread_join(tid1, NULL)<0){ // main은 스레드가 종료될때까지 대기합니다.
                perror("pthread_join");
                exit(1);
        }

        if(argc == 3) { // 만약 파라미터가 3개가 왔다면 파일로 저장까지 해줍니다.
                if (rename(".tempTanslate", argv[2]) < 0)  {
                        perror("rename");
                        exit(1);
                }
        }


        return 0;
}
                            
