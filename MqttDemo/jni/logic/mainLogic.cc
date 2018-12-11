#pragma once
#include "uart/ProtocolSender.h"
#include "net/WifiManager.h"

/*
*此文件由GUI工具生成
*文件功能：用于处理用户的逻辑相应代码
*功能说明：
*========================onButtonClick_XXXX
当页面中的按键按下后系统会调用对应的函数，XXX代表GUI工具里面的[ID值]名称，
如Button1,当返回值为false的时候系统将不再处理这个按键，返回true的时候系统将会继续处理此按键。比如SYS_BACK.
*========================onSlideWindowItemClick_XXXX(int index) 
当页面中存在滑动窗口并且用户点击了滑动窗口的图标后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如slideWindow1;index 代表按下图标的偏移值
*========================onSeekBarChange_XXXX(int progress) 
当页面中存在滑动条并且用户改变了进度后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如SeekBar1;progress 代表当前的进度值
*========================ogetListItemCount_XXXX() 
当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表的总数目,XXX代表GUI工具里面的[ID值]名称，
如List1;返回值为当前列表的总条数
*========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表当前条目下的内容信息,XXX代表GUI工具里面的[ID值]名称，
如List1;pListItem 是贴图中的单条目对象，index是列表总目的偏移量。具体见函数说明
*========================常用接口===============
*LOGD(...)  打印调试信息的接口
*mTextXXXPtr->setText("****") 在控件TextXXX上显示文字****
*mButton1Ptr->setSelected(true); 将控件mButton1设置为选中模式，图片会切换成选中图片，按钮文字会切换为选中后的颜色
*mSeekBarPtr->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1Ptr->refreshListView() 让mListView1 重新刷新，当列表数据变化后调用
*mDashbroadView1Ptr->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度
*
* 在Eclipse编辑器中  使用 “alt + /”  快捷键可以打开智能提示
*/

#include "system/Thread.h"
#include "mqtt/MQTTClient.h"

#define ADDRESS     	"tcp://192.168.1.130:7887"	// 自行修改一下服务器地址
#define PUB_CLIENTID    "ExampleClientPub"
#define SUB_CLIENTID    "ExampleClientSub"
#define TOPIC       	"MQTT Examples"
#define PAYLOAD     	"Hello World!"
#define USER_NAME		"zkswe"
#define USER_PASS		"123"
#define QOS         	1
#define TIMEOUT     	10000L


/**
 * 发布线程
 */
class PubThread : public Thread {
protected:
	virtual bool threadLoop() {
		MQTTClient client;
		MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
		MQTTClient_message pubmsg = MQTTClient_message_initializer;
		MQTTClient_deliveryToken token;
		int rc;

		// 创建连接对象
		MQTTClient_create(&client, ADDRESS, PUB_CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
		conn_opts.keepAliveInterval = 20;
		conn_opts.cleansession = 1;
		conn_opts.username = USER_NAME;		// 用户名称
		conn_opts.password = USER_PASS;		// 密码

		// 连接
		if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
			LOGD("Failed to connect, return code %d\n", rc);
			return false;
		}

		pubmsg.qos = QOS;
		pubmsg.retained = 0;

		int testCount = 1;
		while (testCount <= 10) {
			char msg[50];
			sprintf(msg, "test count %d", testCount);

			pubmsg.payload = msg;	// 消息内容
			pubmsg.payloadlen = strlen(msg);

			// 发布消息
			MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);

			// sleep 2s
			sleep(2000);
			++testCount;
		}

		pubmsg.payload = "Quit";	// 消息内容
		pubmsg.payloadlen = strlen("Quit");

		// 发布退出消息
		MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);

		rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
		LOGD("Message with delivery token %d delivered\n", token);

		MQTTClient_disconnect(client, 10000);
		MQTTClient_destroy(&client);

		return false;
	}
};

static PubThread sPubThread;

volatile MQTTClient_deliveryToken deliveredtoken;

static bool sQuit = false;

static void delivered(void *context, MQTTClient_deliveryToken dt) {
	LOGD("Message with token value %d delivery confirmed\n", dt);
	deliveredtoken = dt;
}

static int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
	LOGD("Message arrived\n");
	LOGD("     topic: %s\n", topicName);
	LOGD("   message: ");

	char msg[message->payloadlen + 1] = { 0 };
	memcpy(msg, message->payload, message->payloadlen);
	LOGD("    %s\n", msg);

	if (!strcmp(msg, "Quit")) {
		sQuit = true;
	}

	mTextview2Ptr->setText(msg);

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);

	return 1;
}

static void connlost(void *context, char *cause) {
	LOGD("\nConnection lost\n");
	LOGD("     cause: %s\n", cause);
}


/**
 * 订阅线程
 */
class SubThread : public Thread {
protected:
	virtual bool threadLoop() {
		MQTTClient client;
		MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
		int rc;
		int ch;

		MQTTClient_create(&client, ADDRESS, SUB_CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
		conn_opts.keepAliveInterval = 20;
		conn_opts.cleansession = 1;
		conn_opts.username = USER_NAME;
		conn_opts.password = USER_PASS;

		MQTTClient_setCallbacks(client, "sub", connlost, msgarrvd, delivered);

		if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
			LOGD("Failed to connect, return code %d\n", rc);
			return false;
		}

		LOGD("Subscribing to topic %s\nfor client %s using QoS%d\n\n", TOPIC, SUB_CLIENTID, QOS);

		// 订阅
		MQTTClient_subscribe(client, TOPIC, QOS);

		sQuit = false;
		while (!sQuit) {
			// do nothing sleep 2s
			sleep(2000);
		}

		MQTTClient_unsubscribe(client, TOPIC);
		MQTTClient_disconnect(client, 10000);
		MQTTClient_destroy(&client);

		return false;
	}
};

static SubThread sSubThread;

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	//{1,  1000},
};

/**
 * 当界面构造时触发
 */
static void onUI_init() {
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
        //TODO
    }
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {

}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {

}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {

}

/**
 * 串口数据回调接口
 */
static void onProtocolDataUpdate(const SProtocolData &data) {

}

/**
 * 定时器触发函数
 * 不建议在此函数中写耗时操作，否则将影响UI刷新
 * 参数： id
 *         当前所触发定时器的id，与注册时的id相同
 * 返回值: true
 *             继续运行当前定时器
 *         false
 *             停止运行当前定时器
 */
static bool onUI_Timer(int id) {
	switch (id) {
	default:
		break;
	}

    return true;
}

/**
 * 有新的触摸事件时触发
 * 参数：ev
 *         新的触摸事件
 * 返回值：true
 *            表示该触摸事件在此被拦截，系统不再将此触摸事件传递到控件上
 *         false
 *            触摸事件将继续传递到控件上
 */
static bool onmainActivityTouchEvent(const MotionEvent &ev) {
	return false;
}

static bool onButtonClick_Button1(ZKButton *pButton) {
    //LOGD(" ButtonClick Button1 !!!\n");
	// 跳转到wifi设置界面
	EASYUICONTEXT->openActivity("WifiSettingActivity");
    return false;
}

static bool onButtonClick_PublishButton(ZKButton *pButton) {
    //LOGD(" ButtonClick PublishButton !!!\n");
	// 启动发布线程
	sPubThread.run();
    return false;
}

static bool onButtonClick_SubscribeButton(ZKButton *pButton) {
    //LOGD(" ButtonClick SubscribeButton !!!\n");
	// 启动订阅线程
	sSubThread.run();
    return false;
}
