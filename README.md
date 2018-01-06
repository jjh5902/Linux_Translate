# Linux file translate!

## 리눅스 파일 번역 프로그램
- 브라우저를 통해 번거롭게 번역하지 않아도 프로그램 실행을 통해 구글 번역 API를 통해 영문으로 작성된 파일을 한글로 번역해 줍니다.
- 영문에서 바로 한글로 번역하지 않고, 영문에서 일본어, 일본어에서 한글로 번역하기에 더욱 정확한 결과가 나옵니다.

## 반드시 있어야 하는 파일
- linuxProject.json // 구글 번역 API key 파일은 직접 발급받아야 합니다.
- requirements.txt
- myTranslate.c


## gcc 컴파일 및 파일 실행하기전에 설치하는 방법
sudo apt install python-minimal
sudo apt-get install python-dev
sudo apt install python-pip
sudo pip install -r requirements.txt
export GOOGLE_APPLICATION_CREDENTIALS=/home/linux/project/linuxProject.json

## gcc 컴파일 방법
gcc -o myTranslate myTranslate.c -lpython2.7 -lpthread

## 프로그램 실행방법
### 번역본을 stdout으로만 보고 싶을때
./myTranslate input
ex) ./myTranslate MartinLuthorKing_Speech.txt

### 번역본을 stdout로도 보고 파일로도 저장하고 싶을때
./myTranslate input output
ex)./myTranslate MartinLuthorKing_Speech.txt MartinLuthorKing_Speech_translate.txt
