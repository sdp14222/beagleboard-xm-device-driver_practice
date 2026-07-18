# 개요
---
## 목적
---
- 디바이스 드라이버 실습

## 개발 환경
---
- Ubuntu 18.04

## 개발 보드
---
- BeagleBoard XM

## 포팅 및 커널
---
- 포팅 기본 참고 방법
  - [RobertCNelson](https://github.com/RobertCNelson/armv7-multiplatform/tree/v5.4.x?tab=readme-ov-file)
    - v5.4.x branch 기준
      - [sdp14222/beagleboard-xm-porting](https://github.com/sdp14222/beagleboard-xm-porting) tmp 브랜치
    - v5.4.x branch 기준 일부 수정 브랜치
      - [sdp14222/beagleboard-xm-porting](https://github.com/sdp14222/beagleboard-xm-porting) v5.4.x_tmp 브랜치
- 기준 리눅스 커널
  - 기준 브랜치 : [sdp14222/beagleboard-xm-kernel](https://github.com/sdp14222/beagleboard-xm-kernel) v5.4.288-armv7-x69 브랜치
  - 일부 수정 브랜치 : [sdp14222/beagleboard-xm-kernel](https://github.com/sdp14222/beagleboard-xm-kernel) v5.4.288-armv7-x69_tmp 브랜치


# 소스 구성
---
## 기준 커널
---
- device_tree_origin
  - 리눅스 커널 기준 브랜치 
- device_tree_modified
  - 리눅스 커널 수정 브랜치 
  - Device Tree 수정

# 참고 자료
---
- [노션 내용 정리](https://app.notion.com/p/Beagle-Board-XM-3975b12fa50580fb88d2dd1845f180d0)
