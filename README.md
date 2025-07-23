# 다운로드 방법
<> Code라고 적힌 버튼을 누르면 HTTPS 형식의 URL을 확인할 수 있습니다.
URL 오른쪽에 사각형 두 개가 겹쳐있는 모양의 버튼을 누르면 URL을 확인할 수 있습니다.
터미널을 켜신 이후 코드를 다운로드 받기 원하는 위치로 이동해주신 후, `git clone`을 해주면 소스 코드를 다운로드 받을 수 있습니다.
```
git clone https://github.com/PNU-HIPEx/MPSS.git
```

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
./xRayGeant4.out --vis true
```
를, GUI 없이 시뮬레이션을 실행하기 위해서는
```
./xRayGeant4.cout
```
를 실행해줍니다.
