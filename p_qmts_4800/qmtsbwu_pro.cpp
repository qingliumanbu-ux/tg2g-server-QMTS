/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     wsl
Version:    1.0
Date:       2024-01-6
Description: 推送聊天配置保存
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件


BM2F_ENTERACE(qmtsbwu_pro)

int f_qmtsbwu_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	int seq_no = 0;
	CString procDiv = "";
	int code_count = 0;
	CString v_operate = "";
	CString uid = " ";
	CModel tqmtsbwu("TQMTSBWU");
	CModel tqmtsbwu_yh("TQMTSBWU_YH");
	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("PRO_DIV"))
		{
			v_operate = bcls_rec->Tables[0].Rows[0]["PRO_DIV"].ToString().Trim();
		}
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tqmtsbwu.Reset();
			tqmtsbwu.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (v_operate == "ADD")
			{
				//查询代码是否已存在
				CString sqlstr = "SELECT COUNT(*) FROM TQMTSBWU WHERE CODE= '" + bcls_rec->Tables[0].Rows[0]["CODE"].ToString().Trim() + "'";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					code_count = cmd_inq.GetInt16(1);
				}
				cmd_inq.Close();
				Log::Trace("", "", "开始,count=[{0}]", code_count);
				if (code_count != 0)
				{
					strcpy(s.msg, _RES("代码已存在")/*代码已存在。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables[0].Rows.get_Count());
				tqmtsbwu["REC_CREATOR"] = s.userid;
				tqmtsbwu["REC_CREATE_TIME"] = dateNow;
				tqmtsbwu["KZABSCHL"] = "1";
				tqmtsbwu.TrimOrBlank();
				tqmtsbwu.Insert();
			}
			else if (v_operate == "UPD")
			{
				tqmtsbwu["DU_MAKER"] = s.userid;
				tqmtsbwu["DU_TIME"] = dateNow;
				tqmtsbwu.TrimOrBlank();
				tqmtsbwu.Update("DU_MAKER,DU_TIME,LOGINNAME,MESSAGE_LIST,MESSAGE_TITLE,MODULE_NAME,KZABSCHL", "CODE");
			}

		}

		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				tqmtsbwu.Reset();
				tqmtsbwu.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);

				if (tqmtsbwu["CODE"].ToString().TrimOrBlank() == " ")
				{
					strcpy(s.msg, _RES("代码不能为空")/*代码不能为空。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tqmtsbwu.Delete("CODE"); //条件字段项

				tqmtsbwu_yh.Reset();
				tqmtsbwu_yh.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);
				tqmtsbwu_yh.Delete("CODE");
			}
		}
		if (bcls_rec->Tables.Contains("B_ADD") && bcls_rec->Tables.Contains("ROW_CODE"))
		{
			Log::Trace("", "", "新增开始1,count=[{0}]", bcls_rec->Tables["B_ADD"].Rows.get_Count());
			
			for (int i = 0; i < bcls_rec->Tables["B_ADD"].Rows.get_Count(); i++)
			{
				//查询用户是否存在
				CString sqlstr = "SELECT USER_ID FROM TQMTSBWU_YH WHERE USER_ID='" + bcls_rec->Tables["B_ADD"].Rows[i]["USER_ID"].ToString() + "' AND CODE='" + bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString() + "'";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					uid = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
				Log::Trace("", "", "开始,uid=[{0}]", uid);
				if (uid.TrimOrBlank() != " ")
				{
					strcpy(s.msg, _RES(uid + "用户已存在")/*用户已存在。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//查询序列号最大
				CString sqlstr1 = "SELECT MAX(SEQ_NO) FROM TQMTSBWU_YH WHERE CODE='" + bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString() + "'";
				cmd_inq1.SetCommandText(sqlstr1);
				cmd_inq1.ExecuteReader();
				if (cmd_inq1.Read())
				{
					seq_no = cmd_inq1.GetInt16(1);
				}
				cmd_inq1.Close();
				tqmtsbwu_yh.MergeFrom(bcls_rec->Tables["B_ADD"].Rows[i]);

				tqmtsbwu_yh["REC_CREATOR"] = s.userid;
				tqmtsbwu_yh["REC_CREATE_TIME"] = dateNow;
				tqmtsbwu_yh["SEQ_NO"] = seq_no + 1;
				tqmtsbwu_yh["CODE"] = bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString();
				tqmtsbwu_yh.Insert();

			}
			//把用户信息新增到主表
			CString str = "";
			CString sqlstr = "SELECT USER_ID FROM TQMTSBWU_YH WHERE CODE= '" + bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString() + "'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				str += cmd_inq.GetString(1) + ",";
			}
			cmd_inq.Close();
			Log::Trace("", "", "开始,str=[{0}]", str);
			//去掉第一个逗号，和最后一个逗号以及空格
			CString spare_item_3 = str;
			Log::Trace("", "", "开始,spare_item_3=[{0}]", spare_item_3);
			if (spare_item_3.Trim() != "")
			{
				spare_item_3 = spare_item_3.Trim();
				if (spare_item_3.Substring(0, 1) == "," || spare_item_3.Substring(0, 1) == "，")
				{
					spare_item_3 = spare_item_3.Substring(1, spare_item_3.GetLength() - 1);
				}
				if (spare_item_3.Substring(spare_item_3.GetLength() - 1, 1) == "," || spare_item_3.Substring(spare_item_3.GetLength() - 1, 1) == "，")
				{
					spare_item_3 = spare_item_3.Substring(0, spare_item_3.GetLength() - 1);
				}
			}
			Log::Trace("", "", "结束,spare_item_3=[{0}]", spare_item_3);
			if (spare_item_3.GetLength() > 4000)
			{
				spare_item_3 = " ";
			}
			tqmtsbwu["LOGINNAME_LIST"] = spare_item_3;
			tqmtsbwu["CODE"] = bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString();
			tqmtsbwu.Update("LOGINNAME_LIST", "CODE");
		}
		if (bcls_rec->Tables.Contains("B_UPD") && bcls_rec->Tables.Contains("ROW_CODE"))
		{
			Log::Trace("", "", "修改开始1,count=[{0}]", bcls_rec->Tables["B_UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["B_UPD"].Rows.get_Count(); i++)
			{
				tqmtsbwu_yh.MergeFrom(bcls_rec->Tables["B_UPD"].Rows[i]);
				tqmtsbwu_yh.Delete("SEQ_NO");

				tqmtsbwu_yh.MergeFrom(bcls_rec->Tables["B_UPD"].Rows[i]);
				tqmtsbwu_yh.Insert();
			}
			CString str = "";
			CString sqlstr = "SELECT USER_ID FROM TQMTSBWU_YH WHERE CODE= '" + bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString() + "'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				str += cmd_inq.GetString(1) + ",";
			}
			cmd_inq.Close();
			Log::Trace("", "", "开始,str=[{0}]", str);
			//去掉第一个逗号，和最后一个逗号以及空格
			CString spare_item_3 = str;
			Log::Trace("", "", "开始1,spare_item_3=[{0}]", spare_item_3);
			if (spare_item_3.Trim() != "")
			{
				spare_item_3 = spare_item_3.Trim();
				if (spare_item_3.Substring(0, 1) == "," || spare_item_3.Substring(0, 1) == "，")
				{
					spare_item_3 = spare_item_3.Substring(1, spare_item_3.GetLength() - 1);
				}
				if (spare_item_3.Substring(spare_item_3.GetLength() - 1, 1) == "," || spare_item_3.Substring(spare_item_3.GetLength() - 1, 1) == "，")
				{
					spare_item_3 = spare_item_3.Substring(0, spare_item_3.GetLength() - 1);
				}
			}
			Log::Trace("", "", "结束1,spare_item_3=[{0}]", spare_item_3);
			if (spare_item_3.GetLength() > 4000)
			{
				spare_item_3 = " ";
			}
			tqmtsbwu["LOGINNAME_LIST"] = spare_item_3;
			tqmtsbwu["CODE"] = bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString();
			tqmtsbwu.Update("LOGINNAME_LIST", "CODE");
		}
		if (bcls_rec->Tables.Contains("B_DEL") && bcls_rec->Tables.Contains("ROW_CODE"))
		{
			Log::Trace("", "", "删除开始1,count=[{0}]", bcls_rec->Tables["B_DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["B_DEL"].Rows.get_Count(); i++)
			{
				tqmtsbwu_yh.MergeFrom(bcls_rec->Tables["B_DEL"].Rows[i]);

				if (tqmtsbwu_yh["SEQ_NO"].ToString().TrimOrBlank() == " ")
				{
					strcpy(s.msg, _RES("序号不能为空")/*代码不能为空。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tqmtsbwu_yh.Delete("SEQ_NO,CODE"); //条件字段项
			}
			CString str = "";
			CString sqlstr = "SELECT USER_ID FROM TQMTSBWU_YH WHERE CODE= '" + bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString() + "'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				str += cmd_inq.GetString(1) + ",";
			}
			cmd_inq.Close();
			//去掉第一个逗号，和最后一个逗号以及空格
			CString spare_item_3 = str;
			Log::Trace("", "", "开始2,spare_item_3=[{0}]", spare_item_3);
			if (spare_item_3.Trim() != "")
			{
				spare_item_3 = spare_item_3.Trim();
				if (spare_item_3.Substring(0, 1) == "," || spare_item_3.Substring(0, 1) == "，")
				{
					spare_item_3 = spare_item_3.Substring(1, spare_item_3.GetLength() - 1);
				}
				if (spare_item_3.Substring(spare_item_3.GetLength() - 1, 1) == "," || spare_item_3.Substring(spare_item_3.GetLength() - 1, 1) == "，")
				{
					spare_item_3 = spare_item_3.Substring(0, spare_item_3.GetLength() - 1);
				}
			}
			Log::Trace("", "", "结束2,spare_item_3=[{0}]", spare_item_3);
			tqmtsbwu.Reset();
			tqmtsbwu.MergeFrom(bcls_rec->Tables["ROW_CODE"].Rows[0]);
			tqmtsbwu["DU_MAKER"] = s.userid;
			tqmtsbwu["DU_TIME"] = dateNow;
			if (spare_item_3.GetLength() > 4000)
			{
				spare_item_3 = " ";
			}
			tqmtsbwu["LOGINNAME_LIST"] = spare_item_3;
			tqmtsbwu.TrimOrBlank();
			tqmtsbwu.Update("DU_MAKER,DU_TIME,LOGINNAME,MESSAGE_LIST,MESSAGE_TITLE,MODULE_NAME,LOGINNAME_LIST", "CODE");

		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


