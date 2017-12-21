#include <stdio.h>
#include <python2.7/Python.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#define MAX_BUF 1024

pthread_t tid1;

void SigHandler(int signo){
	if(signo == SIGINT){
		Py_Initialize();
		if(Py_IsInitialized()){
			PyRun_SimpleString("print('Translate Stop')");
               		PyRun_SimpleString("with open('.tempTanslate', 'w') as tt:\n    tt.writelines('\\n'.join(tlStr))");
			PyRun_SimpleString("print('SAVE UNTIL INPUT SIGINT')");
		}
		pthread_cancel(tid1);
	}
	exit(0);
}

void PyTranslate(FILE* src){
   char buf[MAX_BUF];

   Py_Initialize();
        if(Py_IsInitialized()) {
                PyRun_SimpleString("print('I am Python2.7!')");
                PyRun_SimpleString("from google.cloud import translate");
                PyRun_SimpleString("translate_client = translate.Client()");
                PyRun_SimpleString("target = 'jpn'");
                PyRun_SimpleString("target2 = 'ko'");
                PyRun_SimpleString("tlStr = []");
                PyRun_SimpleString("print 'Translation:'");
		PyRun_SimpleString("print('Translate to Japanese End')");
		PyRun_SimpleString("print('Translate to Korea End')");
                while(fgets(buf, MAX_BUF, src) != NULL) {
                        buf[strlen(buf) - 1] = '\0';
                        char mystr[MAX_BUF+10];
                        sprintf(mystr, "text = u'%s'", buf);
                        PyRun_SimpleString(mystr);
                        PyRun_SimpleString("translation = translate_client.translate(text, target_language=target)");
                        PyRun_SimpleString("text2 = translation['translatedText']");
                        PyRun_SimpleString("translation = translate_client.translate(text2, target_language=target2)");
                        PyRun_SimpleString("print(u'{}'.format(translation['translatedText']))");
                        PyRun_SimpleString("tlStr += ['{}'.format(translation['translatedText'].encode('utf-8'))]");
                }
		PyRun_SimpleString("print('Translate finish')");
                PyRun_SimpleString("with open('.tempTanslate', 'w') as tt:\n    tt.writelines('\\n'.join(tlStr))");

        }
        Py_Finalize();

}


int main(int argc, char *argv[])
{
	signal(SIGINT, SigHandler);
        FILE *src;

        if(argc != 2 && argc != 3) {
                fprintf(stderr, "Usage: %s source destination\n", argv[0]);
                exit(1);
        }
        if ((src = fopen(argv[1], "rt")) == NULL)  {
                perror("fopen");
                exit(1);
        }

        if(pthread_create(&tid1, NULL, (void*)PyTranslate, (void*)src)<0){
                perror("pthread_create");
                exit(1);
        }

        if(pthread_join(tid1, NULL)<0){
                perror("pthread_join");
                exit(1);
        }

        if(argc == 3) {
                if (rename(".tempTanslate", argv[2]) < 0)  {
                        perror("rename");
                        exit(1);
                }
        }


        return 0;
}
                            
