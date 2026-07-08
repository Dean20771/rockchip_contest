# STM32与ROS通信协议说明文档

## 一、通信方式

- **接口**: USART1（串口1）
- **协议**: 文本协议 (Text Mode)
- **波特率**: 根据USART1配置（请在CubeMX中确认）
- **数据位**: 8位
- **停止位**: 1位
- **校验位**: 无

---

## 二、机器人参数配置（重要！）

### 2.1 配置位置

所有参数在 `main.c` 的 `USER CODE BEGIN PV` 区域（约第64-86行）

### 2.2 必须配置的参数

#### **1. 机器人类型**
```c
#define ROBOT_TYPE  0  // 0=差速驱动, 1=麦克纳姆轮, 2=全向轮
```

| 值 | 类型 | 说明 |
|----|------|------|
| 0 | 差速驱动 | 两轮独立驱动（最常见） |
| 1 | 麦克纳姆轮 | 四个麦克纳姆轮，可全向移动 |
| 2 | 全向轮 | 三轮或四轮全向轮布局 |

#### **2. 轮子参数**
```c
#define WHEEL_DIAMETER   0.065f   // 轮子直径 (米)
#define WHEEL_BASE       0.200f   // 轮距 (米)
#define WHEEL_TRACK      0.200f   // 轴距 (米) - 麦克纳姆轮需要
```

**如何测量**:
- **WHEEL_DIAMETER**: 用卡尺测量轮子外径，单位转换为米
  - 示例：65mm = 0.065m
- **WHEEL_BASE**: 测量左右轮中心之间的距离
  - 差速驱动：左右轮中心距离
  - 麦克纳姆轮：左右轮中心距离
- **WHEEL_TRACK**: 前后轮中心之间的距离（仅麦克纳姆轮需要）

#### **3. 编码器参数**
```c
#define ENCODER_PPR      1024     // 编码器每转脉冲数
#define GEAR_RATIO       30.0f    // 减速比
```

**如何确定**:
- **ENCODER_PPR**: 查看编码器规格书
  - 常见值：360, 512, 1024, 2000, 2048
  - 注意：四倍频模式会变成 4×PPR
- **GEAR_RATIO**: 电机减速器铭牌上标注
  - 常见值：10, 20, 30, 50, 100
  - 如果没有减速器，设为 1.0

#### **4. 采样周期**
```c
#define DT  0.1f  // 采样周期 (秒)
```
**必须与发送频率同步**:
- `HAL_Delay(100)` → `DT = 0.1f` (100ms)
- `HAL_Delay(50)` → `DT = 0.05f` (50ms)
- `HAL_Delay(200)` → `DT = 0.2f` (200ms)

### 2.3 配置示例

#### 示例1：小型差速驱动机器人
```c
#define ROBOT_TYPE       0         // 差速驱动
#define WHEEL_DIAMETER   0.065f    // 65mm轮子
#define WHEEL_BASE       0.180f    // 18cm轮距
#define ENCODER_PPR      1024      // 1024线编码器
#define GEAR_RATIO       30.0f     // 1:30减速比
#define DT               0.1f      // 100ms采样
```

#### 示例2：大型差速驱动机器人
```c
#define ROBOT_TYPE       0         // 差速驱动
#define WHEEL_DIAMETER   0.150f    // 150mm轮子
#define WHEEL_BASE       0.400f    // 40cm轮距
#define ENCODER_PPR      2048      // 2048线编码器
#define GEAR_RATIO       20.0f     // 1:20减速比
#define DT               0.05f     // 50ms采样
```

#### 示例3：麦克纳姆轮机器人
```c
#define ROBOT_TYPE       1         // 麦克纳姆轮
#define WHEEL_DIAMETER   0.100f    // 100mm轮子
#define WHEEL_BASE       0.300f    // 30cm左右轮距
#define WHEEL_TRACK      0.300f    // 30cm前后轴距
#define ENCODER_PPR      512       // 512线编码器
#define GEAR_RATIO       50.0f     // 1:50减速比
#define DT               0.05f     // 50ms采样
```

### 2.4 编码器接入（必须实现）

在 `Update_Odometry()` 函数中（第117-119行）有以下TODO:

```c
// TODO: 从编码器获取当前计数值，请根据实际硬件修改
// 示例：encoder.left = __HAL_TIM_GET_COUNTER(&htimX);
// 示例：encoder.right = __HAL_TIM_GET_COUNTER(&htimY);
```

**根据您的定时器配置修改**:

```c
// 差速驱动示例（两个编码器）
encoder.left = (int32_t)__HAL_TIM_GET_COUNTER(&htim3);   // 左轮接TIM3
encoder.right = (int32_t)__HAL_TIM_GET_COUNTER(&htim4);  // 右轮接TIM4

// 麦克纳姆轮示例（四个编码器）
encoder.front_left = (int32_t)__HAL_TIM_GET_COUNTER(&htim2);
encoder.front_right = (int32_t)__HAL_TIM_GET_COUNTER(&htim3);
encoder.left = (int32_t)__HAL_TIM_GET_COUNTER(&htim4);   // 后左
encoder.right = (int32_t)__HAL_TIM_GET_COUNTER(&htim8);  // 后右
```

**注意事项**:
1. 确认定时器已配置为编码器模式（Encoder Mode）
2. 确认编码器方向是否需要取反（如果前进时计数减少，加负号）
3. 如果使用16位定时器，注意处理溢出（0→65535 或 65535→0）

### 2.5 快速验证参数是否正确

**步骤1**: 编译上传程序

**步骤2**: 用串口助手连接，观察数据

**步骤3**: 手动推动机器人前进1米，观察 `x` 值

- ✅ 如果 `x` 接近 1.0，参数正确
- ❌ 如果 `x` 远大于或小于 1.0，调整 `WHEEL_DIAMETER` 或 `ENCODER_PPR` 或 `GEAR_RATIO`

**步骤4**: 原地旋转机器人360°，观察 `th` 值

- ✅ 如果 `th` 接近 6.28 (2π)，参数正确
- ❌ 如果差异较大，调整 `WHEEL_BASE`

---

## 三、数据发送格式（STM32 → ROS）

### 3.1 里程计反馈数据格式

```
$x,y,th,vx,vy,wz#
```

### 3.2 字段说明

| 字段 | 含义 | 单位 | 数据类型 | 精度 |
|------|------|------|----------|------|
| `x`  | 累积X坐标 | 米(m) | float | 小数点后3位 |
| `y`  | 累积Y坐标 | 米(m) | float | 小数点后3位 |
| `th` | 姿态角度(Theta) | 弧度(rad) | float | 小数点后3位 |
| `vx` | X方向线速度 | 米/秒(m/s) | float | 小数点后3位 |
| `vy` | Y方向线速度 | 米/秒(m/s) | float | 小数点后3位 |
| `wz` | Z轴角速度(偏航角速度) | 弧度/秒(rad/s) | float | 小数点后3位 |

### 3.3 发送示例

```
$0.123,0.456,1.570,0.500,0.000,0.314#
$-0.050,0.100,3.141,-0.200,0.050,-0.157#
$0.000,0.000,0.000,0.000,0.000,0.000#
```

**解读第一条数据**:
- 位置: X=0.123m, Y=0.456m
- 角度: θ=1.570rad (约90°)
- 速度: vx=0.500m/s, vy=0.000m/s
- 角速度: wz=0.314rad/s

### 3.4 发送频率

- **默认**: 100ms (10Hz)
- **修改位置**: `main.c` 第199行 `HAL_Delay(100);`
- **建议范围**: 50ms~200ms (20Hz~5Hz)

---

## 四、数据接收格式（ROS → STM32）

### 4.1 速度控制指令格式（建议）

如果需要从ROS接收速度控制指令，可采用以下格式：

```
@vx,vy,wz#
```

| 字段 | 含义 | 单位 |
|------|------|------|
| `vx` | 目标X方向线速度 | m/s |
| `vy` | 目标Y方向线速度 | m/s |
| `wz` | 目标角速度 | rad/s |

**示例**:
```
@0.500,0.000,0.314#  // 前进0.5m/s，左转0.314rad/s
@-0.300,0.000,0.000# // 后退0.3m/s
@0.000,0.000,0.000#  // 停止
```

---

## 五、代码中的关键变量

### 5.1 里程计数据结构

在 `main.c` 的 `USER CODE BEGIN PV` 区域定义：

```c
typedef struct {
    float x;   // 累积坐标X (m)
    float y;   // 累积坐标Y (m)
    float th;  // 角度Theta (rad)
    float vx;  // X方向线速度 (m/s)
    float vy;  // Y方向线速度 (m/s)
    float wz;  // Z轴角速度 (rad/s)
} OdomData_t;

OdomData_t odom_data = {0};
```

### 5.2 更新里程计数据示例

在您的里程计计算代码中更新这些变量：

```c
// 示例：根据编码器和IMU数据计算里程计
odom_data.x += delta_x;      // 累积X位移
odom_data.y += delta_y;      // 累积Y位移
odom_data.th = current_theta; // 当前角度
odom_data.vx = velocity_x;    // 当前X速度
odom_data.vy = velocity_y;    // 当前Y速度
odom_data.wz = angular_vel;   // 当前角速度
```

### 5.3 发送函数

```c
void Send_GyroData(void); // 在while循环中每100ms自动调用
```

---

## 六、ROS端配置

### 6.1 串口配置示例（Python）

```python
import serial

# 配置串口
ser = serial.Serial(
    port='/dev/ttyUSB0',      # Linux下串口设备
    baudrate=115200,           # 波特率（根据STM32配置修改）
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    timeout=1
)

# 读取数据
def read_odom_data():
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').strip()
        if line.startswith('$') and line.endswith('#'):
            # 去除起始符和结束符
            data_str = line[1:-1]
            # 解析数据
            values = data_str.split(',')
            if len(values) == 6:
                x, y, th, vx, vy, wz = map(float, values)
                print(f"位置: ({x}, {y}), 角度: {th}, 速度: ({vx}, {vy}, {wz})")
                return x, y, th, vx, vy, wz
    return None

# 发送速度指令
def send_velocity(vx, vy, wz):
    cmd = f"@{vx:.3f},{vy:.3f},{wz:.3f}#\n"
    ser.write(cmd.encode('utf-8'))
```

### 6.2 ROS节点示例

```python
#!/usr/bin/env python3
import rospy
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Twist
import serial
import tf

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

def odom_publisher():
    rospy.init_node('stm32_odom_node')
    odom_pub = rospy.Publisher('odom', Odometry, queue_size=10)
    rate = rospy.Rate(10)  # 10Hz

    while not rospy.is_shutdown():
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            if line.startswith('$') and line.endswith('#'):
                try:
                    data = line[1:-1].split(',')
                    x, y, th, vx, vy, wz = map(float, data)

                    # 创建里程计消息
                    odom = Odometry()
                    odom.header.stamp = rospy.Time.now()
                    odom.header.frame_id = "odom"
                    odom.child_frame_id = "base_link"

                    # 位置
                    odom.pose.pose.position.x = x
                    odom.pose.pose.position.y = y
                    odom.pose.pose.position.z = 0.0

                    # 姿态（四元数）
                    quat = tf.transformations.quaternion_from_euler(0, 0, th)
                    odom.pose.pose.orientation.x = quat[0]
                    odom.pose.pose.orientation.y = quat[1]
                    odom.pose.pose.orientation.z = quat[2]
                    odom.pose.pose.orientation.w = quat[3]

                    # 速度
                    odom.twist.twist.linear.x = vx
                    odom.twist.twist.linear.y = vy
                    odom.twist.twist.angular.z = wz

                    odom_pub.publish(odom)
                except Exception as e:
                    rospy.logwarn(f"解析数据失败: {e}")

        rate.sleep()

if __name__ == '__main__':
    try:
        odom_publisher()
    except rospy.ROSInterruptException:
        pass
    finally:
        ser.close()
```

---

## 七、调参指南

### 7.1 发送频率调整

**位置**: `main.c` 第199行

```c
HAL_Delay(100);  // 修改此值来调整发送频率
```

| 延时(ms) | 频率(Hz) | 适用场景 |
|---------|---------|---------|
| 20      | 50Hz    | 高速运动，需要高精度反馈 |
| 50      | 20Hz    | 一般运动，推荐设置 |
| 100     | 10Hz    | 低速运动，默认设置 |
| 200     | 5Hz     | 静止或超低速，节省带宽 |

### 7.2 数据精度调整

**位置**: `main.c` 第84行

```c
len = sprintf(tx_buffer, "$%.3f,%.3f,%.3f,%.3f,%.3f,%.3f#",
              odom_data.x, odom_data.y, odom_data.th,
              odom_data.vx, odom_data.vy, odom_data.wz);
```

**修改精度**:
- `%.3f` → 保留3位小数（默认，推荐）
- `%.2f` → 保留2位小数（节省带宽）
- `%.4f` → 保留4位小数（高精度需求）

### 7.3 串口波特率

**修改位置**: STM32CubeMX中USART1配置

**常用波特率**:
- 9600 bps - 低速通信
- 115200 bps - 标准配置（推荐）
- 460800 bps - 高速通信
- 921600 bps - 超高速通信

**注意**: ROS端配置必须与STM32一致！

---

## 八、测试与调试

### 8.1 串口助手测试

使用串口调试助手连接STM32：

1. 设置对应波特率
2. 选择文本模式
3. 观察接收到的数据格式是否为 `$x,y,th,vx,vy,wz#`
4. 检查数据是否合理

### 8.2 常见问题排查

| 问题 | 可能原因 | 解决方法 |
|------|---------|---------|
| 收不到数据 | 波特率不匹配 | 检查两端波特率设置 |
| 数据乱码 | 串口参数错误 | 确认8N1配置 |
| 数据格式错误 | 代码未正确编译 | 重新编译下载程序 |
| 数据全为0 | 未更新odom_data | 检查里程计计算代码 |
| 发送频率异常 | 延时设置问题 | 检查HAL_Delay参数 |

### 8.3 性能监控

**单帧数据长度**: 约40~50字节

**示例**: `$0.123,0.456,1.570,0.500,0.000,0.314#` (共44字节)

**带宽计算**:
- 10Hz × 50字节 = 500字节/秒 = 4000比特/秒
- 在115200bps下占用率: 3.5%（很轻松）

---

## 九、扩展功能建议

### 9.1 添加校验和（可选）

格式: `$x,y,th,vx,vy,wz*checksum#`

```c
// 简单的异或校验
uint8_t calc_checksum(char *data) {
    uint8_t checksum = 0;
    while(*data) checksum ^= *data++;
    return checksum;
}
```

### 9.2 添加时间戳（可选）

格式: `$timestamp,x,y,th,vx,vy,wz#`

```c
uint32_t timestamp = HAL_GetTick();
len = sprintf(tx_buffer, "$%lu,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f#",
              timestamp, odom_data.x, odom_data.y, odom_data.th,
              odom_data.vx, odom_data.vy, odom_data.wz);
```

### 9.3 添加数据类型标识（可选）

支持多种数据类型：

- `$O,x,y,th,vx,vy,wz#` - 里程计数据(Odometry)
- `$I,ax,ay,az,gx,gy,gz#` - IMU数据
- `$S,status,voltage,current#` - 状态数据

---

## 十、注意事项

1. ⚠ **所有角度使用弧度制**，不是角度制
   - 180° = 3.14159 rad
   - 90° = 1.5708 rad

2. ⚠ **坐标系约定**
   - X轴：前进方向
   - Y轴：左侧方向
   - Z轴：向上（右手坐标系）

3. ⚠ **保留USER CODE标签**
   - 所有代码都在USER CODE区域内
   - 使用STM32CubeMX重新生成代码时不会丢失

4. ⚠ **中断安全**
   - 如果在中断中更新odom_data，注意数据同步
   - 考虑使用原子操作或临界区保护

---

## 十一、版本信息

- **文档版本**: v1.0
- **创建日期**: 2026-01-22
- **适用平台**: STM32F4系列 + ROS
- **通信协议**: 文本模式 (Text Mode)

---

## 附录：单位换算参考

### 角度换算
- 1 rad = 57.2958°
- 1° = 0.0174533 rad
- π rad = 180°

### 速度换算
- 1 m/s = 3.6 km/h
- 1 rad/s ≈ 57.3°/s

---

**如有问题，请检查**:
1. 串口配置是否正确
2. 数据格式是否完整
3. odom_data是否正确更新
4. 波特率是否匹配

**祝您调试顺利！** 🚀
