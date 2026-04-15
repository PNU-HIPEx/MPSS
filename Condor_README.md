# MPSS HTCondor Manual

이 문서는 MPSS 프로젝트를 HTCondor에서 실행할 때 필요한 설정, 실행 절차, 그리고 장애 대응 방법을 정리합니다.

## 1) 실행 구조 요약

현재 구성은 아래 순서로 동작합니다.

1. submit host에서 소스 tarball 전송
2. execute node에서 빌드 (`cmake`, `make`)
3. 실행 (`MPSS.out`)
4. 성공 시 scratch 결과를 NAS 경로로 동기화

핵심 파일:
- `MPSS.submit`: Condor 제출 설정
- `build_and_run.sh`: execute node에서 빌드/실행/동기화 수행
- `run_condor.sh`: tar 생성 + submit 실행 편의 스크립트

## 2) 사전 준비

### 2-1. 서버 내 execute node 필수 도구
- `cmake`
- Geant4 (`/opt/geant4/...`)
- ROOT (`/opt/root/...`)

### 2-2. NAS 동기화 경로
현재 기본 경로는 `/mnt/condor_data` 입니다.

Condor 실행 계정(`condor`)이 이 경로에 쓰기 가능해야 합니다.

예시:
```bash
sudo mkdir -p /mnt/condor_data
sudo chown condor:condor /mnt/condor_data
sudo chmod 775 /mnt/condor_data
```

검증:
```bash
sudo -u condor bash -lc 'mkdir -p /mnt/condor_data/test && echo ok > /mnt/condor_data/test/a.txt'
```

주의:
- NFS 환경에서 `setfacl`이 `Operation not supported`면 ACL 미지원입니다.
- 이 경우 `chown/chmod` 또는 NFS 서버 export 정책으로 권한을 열어야 합니다.

## 3) 실행 방법

프로젝트 루트에서:
```bash
./run_condor.sh
```

`run_condor.sh`는 내부적으로 다음을 수행합니다.
```bash
tar -czf MPSS-source.tar.gz CMakeLists.txt config.hpp.in build_and_run.sh exe inc src config init_vis.mac vis.mac
condor_submit MPSS.submit
```

## 4) 결과 확인

### 4-1. 잡 상태
```bash
condor_q
condor_history -limit 5 -af ClusterId ProcId JobStatus ExitCode ExitBySignal
```

### 4-2. 실행 로그
현재 submit 설정은 고정 로그 파일명을 사용합니다.
- `Condor.out`
- `Condor.err`
- `Condor.log`

확인 포인트 (`Condor.out`):
- `Using DATA_DIR=...`
- `NAS_SYNC_DIR=...`
- `Run exit code: 0`
- `Syncing outputs to /mnt/condor_data/<JOB_TAG>`
- `Sync complete: /mnt/condor_data/<JOB_TAG>`

### 4-3. NAS 결과 확인
```bash
ls -al /mnt/condor_data
ls -al /mnt/condor_data/<Cluster>.<Process>
```

기본 `JOB_TAG`는 `$(Cluster).$(Process)` 입니다.

## 5) 자주 바꾸는 설정

`MPSS.submit`에서 주로 수정:

- 실행 인자:
```text
arguments = --threads 10
```

- NAS 동기화 경로:
```text
environment = "BUILD_JOBS=1 NAS_SYNC_DIR=/mnt/condor_data JOB_TAG=$(Cluster).$(Process)"
```

- 리소스 요청:
```text
request_cpus = 1
request_memory = 4096
request_disk = 2048
```

- 실행 노드 조건:
```text
requirements = TARGET.OpSys == "LINUX" && TARGET.HasFileTransfer
```

## 6) 트러블슈팅

### 증상 A: `IDLE`에서 안 넘어감
확인:
```bash
condor_q -better-analyze <cluster>.0
```

주요 원인:
- submit host schedd 주소가 `127.0.0.1`로 광고됨
- negotiator가 schedd 연결 실패 (`Connection refused`)

### 증상 B: `Syncing outputs ...`는 나오는데 NAS에 결과가 없음
확인:
- `Condor.err`에 `Permission denied` 메시지

조치:
- `/mnt/condor_data` 권한 재설정 (`condor` 쓰기 가능)

### 증상 C: `make -j` 관련 오류
`BUILD_JOBS`가 비정상 문자열로 전달될 수 있습니다.
현재 스크립트는 숫자 검증 후 기본값 `1`로 보정합니다.

### 증상 D: 로그가 너무 커서 tail이 느림
필요 라인만 검색:
```bash
grep -nE 'Run exit code|Syncing outputs|Sync complete|Permission denied' Condor.out Condor.err
```

## 7) 운영 권장사항

- 개인 홈 경로 대신 Condor 전용 NAS 경로를 사용
- run별 디렉터리 분리 (`JOB_TAG`)
- scratch에서 계산 후 성공 시 NAS 동기화 유지
- 실패 재현을 위해 `Condor.out/err/log` 보관

## 8) 노트북 연결 없이 계속 실행되는 Job 설정

현재 MPSS.submit에는 disconnected mode 기본 설정이 포함되어 있습니다:
```text
when_to_transfer_output = ON_EXIT
want_graceful_removal = false
```

하지만 **Execute node의 HTCondor 설정**도 필요합니다.

### Execute node 관리자용 설정

Execute node (iridium)의 관리자가 다음을 apply해야 합니다:

**파일: `/etc/condor/config.d/99-disconnected.conf` 생성 또는 기존 설정 수정**

```conf
# Disconnected mode: shadow와의 연결 끊김 후에도 job 계속 실행
DISCONNECTED_SHADOW_TIMEOUT = 3600
MAX_DISCONNECTED_TIME = 86400

# Collector 에러 시에도 이미 시작된 job 유지
CONTINUE_ON_COLLECTOR_ERROR = True

# Node 재부팅 후에도 job 복구 (checkpoint 지원 시)
GRACEFUL_SHUTDOWN = true
```

설정 후 execute node에서:
```bash
sudo systemctl restart condor
```

### 확인 방법

설정이 적용되면:
- Submit host 없이도 job이 계속 실행
- `Condor.log`에 suspended/resumed 이벤트가 안 나타남
- NAS 동기화가 정상 진행

설정 전에는:
- Submit host(노트북)가 꺼지면 job 일시중지
- 다시 켜지면 job 재개
- 불완전한 동기화 가능
