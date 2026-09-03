/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2024
Author:      WSL
Version:     1.0
Date:        2024-07-10 13:30:32
Description: 每天定时推送批处理
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


#include "epex.h"
#include <list>

void f_call_ijudge_svc(CDbConnection* conn, const CString& system_code, const CString& envType, const CString& apiCode, const CString& svc_name, EIClass* blks_in, EIClass* blks_out, bool traceJson);

BM2_FUNCTION_EXPORT

list<CString> splitWithStr(const std::string& str, const std::string& pattern)
{
	Log::Trace("", __FUNCTION__, "开始切割字符串={0}", (CString)str);
	std::list<CString> resVec;
	if ("" == str)
	{
		return resVec;
	}
	//方便截取最后一段数据
	std::string strs = str + pattern;

	size_t pos = strs.find(pattern);
	size_t size = strs.size();
	while (pos != std::string::npos)
	{
		std::string x = strs.substr(0, pos);
		resVec.push_back((CString)x);
		strs = strs.substr(pos + 1, size);
		pos = strs.find(pattern);
	}

	return resVec;
}

int f_push_baowu_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_inq(conn);

	CString code = " ";//代码
	CString MODULE_NAME = " ";//模块名
	CString LOGINNAME = " ";//责任人
	CString LOGINNAME_LIST = " ";//消息通知人
	CString MESSAGE_TITLE = " ";//消息标题
	CString MESSAGE_LIST = " ";//消息内容
	CString KZABSCHL = " ";//推送开关

	/*宝物聊天相关字段*/
	CString msgTitle = " ";
	CString message = " ";//邮件消息体
	CString appId = "ipds";//应用ID
	CString sendUserId = "admin";
	CString msgType = "textcard";
	CString showType = "textcard";//消息类型
	CString configNum = "";
	CString isSend = "1";//填1
	CString title = "火车库存";
	CString sendMode = "1";//模式
	CString secretCode = "cewe";//密钥
	CString msgCategory = "03";//消息类型-待办


	try
	{
		//专业模块代码 -- 防止之后不同程序接入发不同的推送信息的一个判断代码
		if (bcls_rec->Tables[0].Columns.Contains("CODE"))
			code = bcls_rec->Tables[0].Rows[0]["CODE"].ToString();
		Log::Trace("", "", "st_code{0}", code);

		CString date = CDateTime::Now().ToString("yyyy") + "年" + CDateTime::Now().ToString("MM") + "月" + CDateTime::Now().ToString("dd") + "日";
		CString time = CDateTime::Now().ToString("HH") + "点" + CDateTime::Now().ToString("mm") + "分" + CDateTime::Now().ToString("ss") + "秒";
		//获取该条件相关信息
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT  MODULE_NAME,LOGINNAME,LOGINNAME_LIST,MESSAGE_TITLE,MESSAGE_LIST,KZABSCHL "
				"   FROM TQMTSBWU "
				"  WHERE CODE = @code  ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("code", code);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			MODULE_NAME = cmd_inq.GetString(1);//模块名
			LOGINNAME = cmd_inq.GetString(2);//推送人
			LOGINNAME_LIST = cmd_inq.GetString(3);//消息通知人
			MESSAGE_TITLE = cmd_inq.GetString(4); //消息标题
			MESSAGE_LIST = cmd_inq.GetString(5);//消息内容
			KZABSCHL = cmd_inq.GetString(6);//推送开关
		}
		cmd_inq.Close();

		if (KZABSCHL == "1")//KZABSCHL 推送开关按钮
		{
			CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
			CString date = CDateTime::Now().ToString("yyyy") + "年" + CDateTime::Now().ToString("MM") + "月" + CDateTime::Now().ToString("dd") + "日";
			CString time = CDateTime::Now().ToString("HH") + "点" + CDateTime::Now().ToString("mm") + "分" + CDateTime::Now().ToString("ss") + "秒";
			EIClass* inblock = new EIClass();
			EIClass* outblock = new EIClass();


			inblock->Tables[0].set_TableName("SYS");
			inblock->Tables[0].Columns.Add(DT_STRING, "messageTitle");
			inblock->Tables[0].Columns.Add(DT_STRING, "message");
			inblock->Tables[0].Columns.Add(DT_STRING, "appId");
			inblock->Tables[0].Columns.Add(DT_STRING, "sendUserId");
			inblock->Tables[0].Columns.Add(DT_STRING, "msgType");
			inblock->Tables[0].Columns.Add(DT_STRING, "showType");
			inblock->Tables[0].Columns.Add(DT_STRING, "sendMode");
			inblock->Tables[0].Columns.Add(DT_STRING, "secretCode");
			inblock->Tables[0].Columns.Add(DT_STRING, "msgCategory");
			inblock->Tables[0].Columns.Add(DT_STRING, "isSend");
			inblock->Tables[0].Columns.Add(DT_STRING, "configNum");

			inblock->Tables[0].Columns.Add(DT_STRING, "title");
			inblock->Tables[0].Columns.Add(DT_STRING, "btntxt");
			inblock->Tables[0].Columns.Add(DT_STRING, "wxurl");
			inblock->Tables[0].Columns.Add(DT_STRING, "description");
			inblock->Tables[0].Columns.Add(DT_STRING, "appUrl");


			CDataRow& row0 = inblock->Tables[0].Rows.Add();
			row0["messageTitle"] = "火车库存";
			row0["message"] = "<div class = \"gray\">" + date + "</div> <div class=\"div_class\">" + time + "</div><div class=\"highlight\">请及时阅读当日火车库存信息</div>";
			row0["appId"] = appId;
			row0["sendUserId"] = LOGINNAME;
			row0["msgType"] = msgType;
			row0["showType"] = showType;
			row0["sendMode"] = sendMode;
			row0["secretCode"] = secretCode;
			row0["msgCategory"] = msgCategory;
			row0["isSend"] = isSend;
			row0["configNum"] = "";
			row0["title"] = MESSAGE_TITLE;
			row0["btntxt"] = "详细点击";
			row0["wxurl"] = "https://www.baidu.com/";
			row0["description"] = "<div class=\"gray\">" + date + "</div> <div class=\"div_class\">" + time + "</div><div class=\"highlight\">请及时阅读当日火车库存信息</div>";
			row0["appUrl"] = "https://www.baidu.com/";

			inblock->Tables.Add("LOGIN_NAME");
			inblock->Tables[1].Columns.Add(DT_STRING, "LoginNameList");//发送工号(可以多人)

			/*-----开始截取工号，并且循环添加进LOGIN_NAME块  --》开始发送多人《-------*/
			list<CString> a = splitWithStr((string)LOGINNAME_LIST, ",");  //调用截取方法
			list<CString>::iterator it;
			int i = 0;
			for (it = a.begin(); it != a.end(); it++) {
				Log::Trace(" ", __FUNCTION__, "person=[{0}]", *it);
				CString username = (*it);
				inblock->Tables[1].Rows.Add();
				inblock->Tables[1].Rows[i]["LoginNameList"] = username;//截取后的工号添加到LOGIN_NAME块
				i++;
			}

			try
			{
				f_call_ijudge_svc(conn, "TG23M_0003", "1", "SND", "callWeChat", inblock, outblock, true);
			}
			catch (const std::exception&)
			{

			}
			Log::Trace("", __FUNCTION__, "1发送宝武聊天", "");
		}

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


