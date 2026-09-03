/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-04-10
Description:宝武用户导入
**************************************************/
//框架头文件
#include "stdafx.h"

//业务头文件
//外部函数声明
BM2F_ENTERACE(qmtsbwu_dy_pro)

int f_qmtsbwu_dy_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 业务变量 */
	CModel tqmtsbwu("TQMTSBWU");
	CModel tqmtsbwu_yh("TQMTSBWU_YH");
	/* 实体类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_yh(conn);

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString user_id = " ";
	/* 数据库操作类定义 */

	try
	{
		if (bcls_rec->Tables.Contains("ROW_CODE"))
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				Log::Trace("", "Count1", bcls_rec->Tables[0].Rows.get_Count());
				//查询用户是否存在
				CString sqlstr_yh = "SELECT USER_ID FROM TQMTSBWU_YH WHERE USER_ID='" + bcls_rec->Tables[0].Rows[i]["USER_ID"].ToString() + "' AND CODE='" + bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString() + "'";
				cmd_inq_yh.SetCommandText(sqlstr_yh);
				cmd_inq_yh.ExecuteReader();
				if (cmd_inq_yh.Read())
				{
					user_id = cmd_inq_yh.GetString(1);
				}
				cmd_inq_yh.Close();

				if (user_id.TrimOrBlank() != " ")
				{
					strcpy(s.msg, _RES(user_id + "用户已存在")/*用户已存在。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				Log::Trace("", "code", bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString());
				int seq_no = 0;
				//查询序列号最大
				CString sqlstr1 = "SELECT MAX(SEQ_NO) FROM TQMTSBWU_YH WHERE CODE='" + bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString() + "'";
				cmd_inq.SetCommandText(sqlstr1);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					seq_no = cmd_inq.GetInt16(1);
				}
				cmd_inq.Close();

				Log::Trace("", "Count", bcls_rec->Tables[0].Rows.get_Count());

				//每次循环先将数据清空 在merge数据
				tqmtsbwu_yh.Reset();
				tqmtsbwu_yh.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				tqmtsbwu_yh["REC_CREATOR"] = s.userid;
				tqmtsbwu_yh["REC_CREATE_TIME"] = datetime;
				tqmtsbwu_yh["SEQ_NO"] = seq_no + 1;
				tqmtsbwu_yh["CODE"] = bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString();
				tqmtsbwu_yh.TrimOrBlank();
				tqmtsbwu_yh.Insert();
			}
			//把用户信息新增到主表
			CString str = "";
			sqlstr = "SELECT USER_ID FROM TQMTSBWU_YH WHERE CODE= '" + bcls_rec->Tables["ROW_CODE"].Rows[0]["CODE"].ToString() + "'";
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


