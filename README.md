## 컨피그 파일 경로 설정

프로그램을 돌리기 앞서 컨피그 파일 경로를 바꿔주어야 합니다.
exe 디렉토리의 xRayGeant4.cpp라는 파일을 여신 후 13 번 째 줄에 있은 configFilePath 변수에 설정된 값을 파일에 있는 위치에 맞게 설정해주어야 합니다.
컨피그 파일 예제는 config 디렉토리에 있는 simulation.conf 파일입니다.

## 빌드 및 실행

먼저 소스 코드 내에서 빌드 디렉토리를 만들어 준 후 이동해줍니다.
```
mkdir build && cd build
```
그리고 빌드를 위한 환경 설정을 해준 뒤
```
cmake ..
```
빌드를 실행해 줍니다.
```
make
```
파일을 실행해 주는 데, GUI를 사용하려면
```
./xRayGeant4.out --vis
```
를, GUI를 사용하려면 다음을 해주면 됩니다.
```
./xRayGeant4.cout
```
