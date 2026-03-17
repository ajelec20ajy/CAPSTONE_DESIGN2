
import serial
import serial.tools.list_ports
import time
import random
import numpy as np
import matplotlib.pyplot as plt

# 설정 
ITERATIONS = 20       # 총 테스트 횟수
TARGET_RPM = 100.0    # 목표 RPM

# 탐색 범위 (Left)
KP_MIN_L, KP_MAX_L = 1.07, 1.070
KI_MIN_L, KI_MAX_L = 0.05, 0.12

# 탐색 범위 (Right)
KP_MIN_R, KP_MAX_R = 1.082, 1.082
KI_MIN_R, KI_MAX_R = 0.05, 0.12

# 채점 가중치
W_OVERSHOOT = 2.0
W_SETTLING  = 1.0
W_RISE      = 2.0
W_ERROR     = 3.0

ACTIVE_TIME = 2.1     # 데이터 수집 시간
TOTAL_CYCLE = 5.0     # 한 루프당 전체 시간

# 함수 정의
def find_stm32_port():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if "STMicroelectronics" in p.description or "STM32" in p.description or "ST-Link" in p.description:
            return p.device
    return None

def analyze_response(time_data, rpm_data, target):
    """성능 지표 분석 함수"""
    if len(rpm_data) < 20: return 999, 999, 999, 999 

    t = np.array(time_data) / 1000.0 
    y = np.array(rpm_data)
    
    # 1. Steady State Error (마지막 20% 구간)
    tail_idx = int(len(y) * 0.8)
    ess = abs(target - np.mean(y[tail_idx:]))

    # 2. Overshoot
    max_val = np.max(y)
    overshoot = (max_val - target) / target * 100.0 if max_val > target else 0.0

    # 3. Rise Time (10% -> 90%)
    try:
        t_10 = t[np.where(y >= target * 0.1)[0][0]]
        t_90 = t[np.where(y >= target * 0.9)[0][0]]
        rise_time = t_90 - t_10
    except:
        rise_time = 1.5

    # 4. Settling Time (±5%)
    margin = target * 0.05
    out_of_bound = np.where((y > target + margin) | (y < target - margin))[0]
    settling_time = t[out_of_bound[-1] + 1] if len(out_of_bound) > 0 and out_of_bound[-1] < len(y)-1 else 1.5

    return ess, overshoot, rise_time, settling_time

def calculate_cost(ess, os, tr, ts):
    return (os * W_OVERSHOOT) + (ts * W_SETTLING * 10) + (tr * W_RISE * 10) + (ess * W_ERROR)

# 메인루프
if __name__ == "__main__":
    port = find_stm32_port()
    if not port:
        print("STM32 X"); exit()

    results = []
    print(f"연결됨: {port} | 상위 5개 결과 분석 모드")

    try:
        with serial.Serial(port, 115200, timeout=0.1) as ser:
            time.sleep(2)
            ser.reset_input_buffer()

            for i in range(1, ITERATIONS + 1):
                kp_l = round(random.uniform(KP_MIN_L, KP_MAX_L), 3)
                ki_l = round(random.uniform(KI_MIN_L, KI_MAX_L), 3)
                kp_r = round(random.uniform(KP_MIN_R, KP_MAX_R), 3)
                ki_r = round(random.uniform(KI_MIN_R, KI_MAX_R), 3)

                payload = f"PID_L:{kp_l},{ki_l},0.0 PID_R:{kp_r},{ki_r},0.0 CMD:{TARGET_RPM}\n"
                ser.write(payload.encode())
                
                print(f"[{i}/{ITERATIONS}] 전송: L({kp_l}, {ki_l}) R({kp_r}, {ki_r})", flush=True)

                start_rx = time.time()
                logs_t, logs_y_l, logs_y_r = [], [], []
                
                while (time.time() - start_rx) < ACTIVE_TIME:
                    if ser.in_waiting:
                        line = ser.readline().decode(errors='ignore').strip()
                        if line.startswith("DAT:"):
                            try:
                                p = line.replace("DAT:", "").split(',')
                                logs_t.append(int(p[0]))
                                logs_y_l.append(float(p[2]))
                                logs_y_r.append(float(p[3]))
                            except: continue

                # 분석 및 데이터 저장
                res_l = analyze_response(logs_t, logs_y_l, TARGET_RPM) # (ess, os, tr, ts)
                res_r = analyze_response(logs_t, logs_y_r, TARGET_RPM)
                
                cost_l = calculate_cost(*res_l)
                cost_r = calculate_cost(*res_r)
                total_cost = (cost_l + cost_r) / 2.0

                print(f"   ㄴ 분석 완료: Cost={total_cost:.2f}")

                # 지표 데이터를 딕셔너리에 추가 저장
                results.append({
                    'kp_l': kp_l, 'ki_l': ki_l, 'kp_r': kp_r, 'ki_r': ki_r,
                    'score': total_cost, 
                    'metrics_l': res_l, # (ess, os, tr, ts)
                    'metrics_r': res_r, 
                    'data_t': logs_t, 'data_y_l': logs_y_l, 'data_y_r': logs_y_r,
                    'iter': i
                })

                elapsed = time.time() - start_rx
                if TOTAL_CYCLE > elapsed: time.sleep(TOTAL_CYCLE - elapsed)

    except KeyboardInterrupt:
        print("\n 중단.")

    # [4] 상위 5개 출력
    if results:
        sorted_results = sorted(results, key=lambda x: x['score'])
        top_n = min(len(sorted_results), 5)
        top_results = sorted_results[:top_n]

        print("\n" + "="*115)
        print(f"TOP {top_n} 상세 분석 리스트 (좌우 개별 지표)")
        # 헤더 구성: Rank, Test#, Cost, Gain(L), Gain(R), Ess(L/R), OS(L/R), Tr(L/R), Ts(L/R)
        header = f"{'Rank':<4} | {'Test#':<5} | {'Cost':<7} | {'Gain L(p/i)':<13} | {'Gain R(p/i)':<13} | {'Ess L/R':<10} | {'OS L/R(%)':<10} | {'Tr L/R':<10} | {'Ts L/R':<10}"
        print(header)
        print("-" * 115)
        
        for idx, res in enumerate(top_results):
            # 게인 문자열 생성
            gain_l = f"{res['kp_l']}/{res['ki_l']}"
            gain_r = f"{res['kp_r']}/{res['ki_r']}"
            
            # 지표 문자열 생성 (L/R)
            ess_str = f"{res['metrics_l'][0]:.1f}/{res['metrics_r'][0]:.1f}"
            os_str  = f"{res['metrics_l'][1]:.1f}/{res['metrics_r'][1]:.1f}"
            tr_str  = f"{res['metrics_l'][2]:.2f}/{res['metrics_r'][2]:.2f}"
            ts_str  = f"{res['metrics_l'][3]:.2f}/{res['metrics_r'][3]:.2f}"
            
            print(f"{idx+1:<4} | #{res['iter']:<4} | {res['score']:<7.2f} | {gain_l:<13} | {gain_r:<13} | "
                  f"{ess_str:<10} | {os_str:<10} | {tr_str:<10} | {ts_str:<10}")
        print("="*115)

        # 그래프
        fig, axes = plt.subplots(top_n, 1, figsize=(10, 3 * top_n), sharex=True)
        if top_n == 1: axes = [axes]

        for i, res in enumerate(top_results):
            ax = axes[i]
            t_arr = np.array(res['data_t']) / 1000.0
            ax.plot(t_arr, res['data_y_l'], label=f"Left (P:{res['kp_l']}, I:{res['ki_l']})", color='blue')
            ax.plot(t_arr, res['data_y_r'], label=f"Right (P:{res['kp_r']}, I:{res['ki_r']})", color='green', alpha=0.8)
            ax.axhline(TARGET_RPM, color='red', linestyle='--', alpha=0.6)
            ax.set_title(f"Rank {i+1} [Test #{res['iter']}] - Total Cost: {res['score']:.2f}")
            ax.set_ylabel("RPM")
            ax.legend(loc='lower right', fontsize='small')
            ax.grid(True)

        plt.xlabel("Time (s)")
        plt.tight_layout()
        plt.show()