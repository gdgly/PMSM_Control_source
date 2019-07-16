/*
 * wrapper.cpp
 *
 *  Created on: Jun 17, 2019
 *      Author: watashi
 */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"

#include <vector>
#include <string>
#include "math.h"

#include "wrapper.hpp"
#include "LedBlink.hpp"

#include "MathLib.hpp"
#include "PWM.hpp"
#include "UART.hpp"
#include "MotorInfo.hpp"
#include "ArgSensor.hpp"
#include "UiCtrl.hpp"
#include "PID.hpp"

#include "DebugInfo.hpp"

//動作パラメータを設定するファイルを別にする。
bool isDebugMode = false;
//bool isDebugMode = true;
unsigned int debugCount = 720;

bool DebugStartTrig(void);//bool型のhedder宣言ができないかもしれない。どうしよう。

//全てのWrappr関数で叩けるGlobal Object
//System Class
PWM PWM_Object1; //PWMのHWを叩くClass
PWM PWM_Object2;
PWM PWM_Object3;
PWM PWM_Object4;
UART uartob;

//Process Class
MotorInfo Motor; //モータの電圧・電流等を管理、及び座標変換のClass
ArgSensor sensor; //角度を求める機能を持ったclass
PID IqPID;
PID IdPID;
UiCtrl ui_ctrl; //UI入力を処理するclass
DebugInfo Debug;//デバッグ情報かき集め


void cppwrapper(void){
	{//MathLibの生存時間調整(メモリ空けてくれ!!)
		MathLib mathlibrary;//三角関数を取得
		int mathlib_size = 512;//ライブラリのサイズを指定
		mathlibrary.fInit(mathlib_size);
		Motor.setMathLib(mathlibrary);//モータクラスに算術ライブラリを渡す
	}

	IdPID.SetParam(0.5, 0.1, 0);
	IqPID.SetParam(0.5, 0.1, 0);

	//LL_TIM_DisableBRK(TIM1);//こっちは未検証
	//LL_TIM_DisableIT_BRK(TIM1);//効かない

	PWM_Object1.setTIM(TIM1);
	PWM_Object2.setTIM(TIM1);
	PWM_Object3.setTIM(TIM1);
	PWM_Object4.setTIM(TIM1);

	PWM_Object1.setCH(1);
	PWM_Object2.setCH(2);
	PWM_Object3.setCH(3);
	PWM_Object4.setCH(4);

	PWM_Object1.fInit(4000);
	PWM_Object2.fInit(4000);
	PWM_Object3.fInit(4000);
	PWM_Object4.fInit(4000);

	PWM_Object1.f2Duty(0);//50%duty
	PWM_Object2.f2Duty(0);
	PWM_Object3.f2Duty(0);
	PWM_Object4.f2Duty(0);

	LL_GPIO_SetOutputPin(GPIOC, GPIO_PIN_10);
	LL_GPIO_SetOutputPin(GPIOC, GPIO_PIN_11);
	LL_GPIO_SetOutputPin(GPIOC, GPIO_PIN_12);

	ADC_Init();

	while(1){}
}

void MotorPWMTask(int pArg, float pVd, float pVq){//パラメータの物理量は将来的に変える
	//int mathlib_size = Motor.getMathLib().getLibSize();
	Motor.setArg(pArg);
	//Motor.setArgDelta(parg);//ここで誤差Δθを入力すること。
	Motor.setVd(pVd);
	Motor.setVq(pVq);
	Motor.invClarkTransform();
	Motor.invParkTransform();

	PWM_Object1.f2Duty(Motor.getVu());
	PWM_Object2.f2Duty(Motor.getVv());
	PWM_Object3.f2Duty(Motor.getVw());
}


void HighFreqTask(void){

	if (LL_ADC_IsActiveFlag_JEOS(ADC1) == 1)
		{
			LL_ADC_ClearFlag_JEOS(ADC1);

			//エンコーダ読み取り
			float Iu,Iv,Iw;
			//増幅率のバイアス考慮してない。あとで計算すること。
			Iu = LL_ADC_INJ_ReadConversionData12(ADC1, LL_ADC_INJ_RANK_1)/4095;
			Iv = LL_ADC_INJ_ReadConversionData12(ADC1, LL_ADC_INJ_RANK_2)/4095;
			Iw = LL_ADC_INJ_ReadConversionData12(ADC1, LL_ADC_INJ_RANK_3)/4095;
			Motor.setIuvw(Iu, Iv, Iw);

			//Iuvw -> Idqに変換 (Park,Clark変換)
			Motor.parkTransform();
			Motor.clarkTransform();

			float Id, Iq;//あとで使う　今は未使用だからエラー吐くはず。
			Id = Motor.getId();
			Iq = Motor.getIq();

			//推定誤差計算

			//推定位置反映

			//位置センサを叩く
			sensor.ImArg();//強制転流実行時のエンコーダ位置取得

			//指令値入力
			float Vd_input = 0;
			float Vq_input = 0.5f;

			//Id,IqのPID制御 //別関数に引っ越しした方が綺麗かもしれない。
			IdPID.ErrorUpdate(0);
			//IdPID.ErrorAndTimeUpdate(0, pSampleTime);//こちらを使うべき　推奨
			IqPID.ErrorUpdate(0);
			//IqPID.ErrorAndTimeUpdate(0, 0);

			//IO入力?
			LL_ADC_REG_StartConversionSWStart(ADC2);
			float adc_speed = (float)LL_ADC_REG_ReadConversionData12(ADC2)/4095;
			Vq_input = 0;
			Vd_input = adc_speed;//連れ回し運転

			//PWM出力
			MotorPWMTask(Motor.getMathLib().radToSizeCount(sensor.getArg()), Vd_input, Vq_input);//暫定で作った関数

			if(isDebugMode){//デバッグモードで入る処理
				if(DebugStartTrig()){//起動後停止の確認処理
					DebugTask(Iu, Iv, Iw, sensor.getArg());
				}
			}
		}
/*	else
		{
			LL_ADC_WriteReg(ADC1,ISR,0);
		}*/
}

bool DebugStartTrig(void){
	if(ui_ctrl.getState()){
		if(!sensor.GetIsAccelerating()){
			return true;
		}
	}
	return false;
}

void DebugTask(float pIu, float pIv, float pIw, float pArg){
	//他のclass内に持って行く時には、UARTとかDebugのクラスを渡さないとダメかも。
	//Wrapperが持つ関数にしてしまうのが一番うまく行くと思った。

	Debug.SetMotorData(new DebugInfo::SendMotorData(pIu,pIv,pIw,pArg));//デバッグの種類増やしたい時はここで変えてね
	unsigned int VectCount = Debug.GetVectSize();//Debug用にブチ込んだデータの個数
	if(VectCount < debugCount){
		//モータ停止の動作
		BtnActOFF();
		//モータ停止を確認する動作
		if(sensor.getArg() == sensor.getArgOld()){
			//タイマ停止する動作(何回もこれ呼ばれちゃうから)

			PWM_Object1.f2Duty(0);//50%duty
			PWM_Object2.f2Duty(0);
			PWM_Object3.f2Duty(0);
			PWM_Object4.f2Duty(0);

			PWM_Object1.Disable();
			PWM_Object2.Disable();
			PWM_Object3.Disable();
			PWM_Object4.Disable();
			//UARTで転送する動作
			std::vector<DebugInfo::SendMotorData> vectorbuf = Debug.GetVect();
			for(const auto& num : vectorbuf){
				std::string strbuf;

				strbuf.append(std::to_string(num.mIu));
				strbuf.append(",");
				strbuf.append(std::to_string(num.mIv));
				strbuf.append(",");
				strbuf.append(std::to_string(num.mIw));
				strbuf.append(",");
				strbuf.append(std::to_string(num.mEArg));
				strbuf.append(",");
#ifdef Debug_alpha_beta //ifdefじゃなくてパラメタのヘッダを持たせるべきか。
				strbuf.append(std::to_string(num.mIalpha));
				strbuf.append(",");
				strbuf.append(std::to_string(num.mIbeta));
				strbuf.append(",");
#endif
				strbuf.append(std::to_string(num.mId));
				strbuf.append(",");
				strbuf.append(std::to_string(num.mIq));
				strbuf.append("/n");
				uartob.Transmit(strbuf);
			}
			//while(1){}//ここで止める？要検討
		}
	}
	//暫定で作る
}

void BtnAct(void){//強制転流開始へのトリガ 割り込みから叩くためにここでラッパする
	ui_ctrl.BtnAct(); // ON/OFFのトグルスイッチ　BtnActで書き込み、getStateで状態を読む
	sensor.Start_Stop(ui_ctrl.getState());
}

void BtnActOFF(void){//強制転流開始へのトリガOFF 割り込みから叩かないから本来UiCtrlで定義するべき
	ui_ctrl.BtnActOFF(); // OFFのスイッチ　BtnActOFFで書き込み、getStateで状態を読む
	sensor.Start_Stop(ui_ctrl.getState());
}

void BtnActON(void){//強制転流開始へのトリガON 予備で作ってある。使うかは不明
	ui_ctrl.BtnActON(); // ONのトグルスイッチ　BtnActONで書き込み、getStateで状態を読む
	sensor.Start_Stop(ui_ctrl.getState());
}

void ADC_Init()
{
    LL_ADC_Enable( ADC1 );
    LL_ADC_Enable( ADC2 );
    LL_ADC_Enable( ADC3 );
    /* ADC1 Injected conversions end interrupt enabling */
    LL_ADC_ClearFlag_JEOS( ADC1 );
    LL_ADC_EnableIT_JEOS( ADC1 );
}
