/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      wsl
Version:     1.0
Date:        2024-7-19
Description: 宝武推送外部调用
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


#include "epex.h"
#include <list>

void f_call_ijudge_svc(CDbConnection* conn, const CString& system_code, const CString& envType, const CString& apiCode, const CString& svc_name, EIClass* blks_in, EIClass* blks_out, bool traceJson);

BM2F_ENTERACE(qm_push_baowu_wb)

list<CString> splitWithStl(const std::string& str, const std::string& pattern)
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

int f_qm_push_baowu_wb(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	// 创建电文处理对象
	EPEX epex;

	/* 实体类定义 */


	CString sqlstr("");

	/*宝物聊天相关字段*/
	CString message = " ";//邮件消息体
	CString appId = "sids"; //应用ID
	CString msgType = "text";//推送企业微信消息类型
	CString showType = "text";//消息类型
	CString sendMode = "1";//模式
	CString secretCode = "cewe";//密钥
	CString msgCategory = "03";//消息类型-待办
	CString isSend = "1";//填1
	CString configNum = "";//填空
	CString title = "msgTitle";

	CString code = " ";//代码
	CString key = " ";//主键
	CString MODULE_NAME = " ";//模块名
	CString LOGINNAME = " ";//责任人
	CString LOGINNAME_LIST = " ";//消息通知人
	CString MESSAGE_TITLE = " ";//消息标题
	CString MESSAGE_LIST = " ";//消息内容
	CString KZABSCHL = " ";//推送开关
	CDbCommand cmd_inq(conn);
	CDbCommand cmdws_inq(conn);



	CString remark = "";

	CString SEND_FLAG = "";
	CString Messagew_list = "";

	CString raq_mater_inve = "";

	CModel tqmtsbwu_h("TQMTSBWU_H");
	CModel tqmtsbwu("TQMTSBWU");
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	//月日
	CString date_ry = CDateTime::Now().ToString("MM-dd");
	//分钟
	CString date_fz = CDateTime::Now().ToString("HH:mm:ss");

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++){
			//内容
			if (bcls_rec->Tables[0].Columns.Contains("REMARK"))
				remark = bcls_rec->Tables[0].Rows[i]["REMARK"].ToString().Trim();
			Log::Trace("", "", "remark{0}", remark);

			//专业模块代码 -- 防止之后不同程序接入发不同的推送信息的一个判断代码
			if (bcls_rec->Tables[0].Columns.Contains("CODE"))
				code = bcls_rec->Tables[0].Rows[i]["CODE"].ToString();
			Log::Trace("", "", "st_code{0}", code);

			//主键
			if (bcls_rec->Tables[0].Columns.Contains("KEY"))
				key = bcls_rec->Tables[0].Rows[i]["KEY"].ToString();
			Log::Trace("", "", "KEY{0}", key);




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
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT  SEND_FLAG "
					"   FROM TQMTSBWU_H "
					"  WHERE CODE = @code and REMARK = @key ";
				break;
			}
			cmdws_inq.SetCommandText(sqlstr);
			cmdws_inq.Parameters.Set("code", code);
			cmdws_inq.Parameters.Set("key", key);
			cmdws_inq.ExecuteReader();
			if (cmdws_inq.Read())
			{
				SEND_FLAG = cmdws_inq.GetString(1);//已发送标志
			}
			cmdws_inq.Close();
			Log::Trace("", "", "SEND_FLAG{0}", SEND_FLAG);
			if ((SEND_FLAG.Trim() == "0" || SEND_FLAG.Trim() == "") && KZABSCHL == "1")//1已发送，0则发送 KZABSCHL 推送开关按钮
			{
				Log::Trace("", __FUNCTION__, "发送宝武聊天", "");
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
				row0["messageTitle"] = MESSAGE_TITLE;

				row0["message"] = MESSAGE_LIST + remark;


				//整体消息内容
				Messagew_list = row0["message"];
				Log::Trace("", "", "Messagew_list{0}", Messagew_list);

				row0["appId"] = appId;
				row0["sendUserId"] = LOGINNAME;
				row0["msgType"] = msgType;
				row0["showType"] = showType;
				row0["sendMode"] = sendMode;
				row0["secretCode"] = secretCode;
				row0["msgCategory"] = msgCategory;
				row0["isSend"] = isSend;
				row0["title"] = title;
				inblock->Tables.Add("LOGIN_NAME");
				inblock->Tables[1].Columns.Add(DT_STRING, "LoginNameList");//发送工号(可以多人)

				/*-----开始截取工号，并且循环添加进LOGIN_NAME块  --》开始发送多人《-------*/
				list<CString> a = splitWithStl((string)LOGINNAME_LIST, ",");  //调用截取方法
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

					f_call_ijudge_svc(conn, "TG23M_0003", "1", "MSG", "callWeChat", inblock, outblock, true);

					//发送成功信息写入履历表
					tqmtsbwu_h["REC_CREATOR"] = s.userid;
					tqmtsbwu_h["REC_CREATE_TIME"] = dateNow;
					tqmtsbwu_h["LOGINNAME"] = LOGINNAME; //推送人
					tqmtsbwu_h["LOGINNAME_LIST"] = LOGINNAME_LIST;//消息通知人
					tqmtsbwu_h["MESSAGE_LIST"] = Messagew_list; //消息内容
					tqmtsbwu_h["MESSAGE_TITLE"] = MESSAGE_TITLE; //消息标题
					tqmtsbwu_h["CODE"] = code;//代码
					tqmtsbwu_h["MODULE_NAME"] = MODULE_NAME;//模块名
					tqmtsbwu_h["SEND_FLAG"] = "1";//已发送标志
					tqmtsbwu_h["REMARK"] = key;//跟各个业务唯一对应字段
					tqmtsbwu_h.TrimOrBlank();
					tqmtsbwu_h.Insert();

				}
				catch (const std::exception&)
				{

				}
			}
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
