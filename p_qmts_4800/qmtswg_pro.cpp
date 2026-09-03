/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     wsl
Version:    1.0
Date:       2024-04-11
Description:工艺卡违规导入
**************************************************/
//框架头文件
#include "stdafx.h"

//业务头文件
//外部函数声明
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天
BM2F_ENTERACE(qmtswg_pro)

int f_qmtswg_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 业务变量 */
	CModel tqmtswg("TQMTSWG");
	/* 实体类定义 */
	CDbCommand cmd_inq(conn);

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDecimal cd_count = 0;

	/* 数据库操作类定义 */
	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "SHIFT_GROUP_BZ");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "SAP_ZRDW_DESC");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "UNRULL_FLAG_XM");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "UNRULL_FLAG_LX");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "UNRULL_FLAG_ID");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "TRACE_WT_1_DESC");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "ERR_DESC");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "EXAMINER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "JC_RQ_TIME");

	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tqmtswg.Reset();
			tqmtswg.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			CString heat_no = bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "heat_no	= [{0}]", (const char*)heat_no);

			//查询责任单位
			CString	sqlzr = "SELECT COUNT(*) FROM TEP0002 WHERE CODE_CLASS='QMWG01' AND CODE_DESC_1_CONTENT ='" + bcls_rec->Tables[0].Rows[i]["SAP_ZRDW_DESC"].ToString().Trim() + "'";
			Log::Trace("", __FUNCTION__, "sqlzr	= [{0}]", (const char*)sqlzr);
			cmd_inq.Parameters.Clear();
			cmd_inq.SetCommandText(sqlzr);
			cd_count = cmd_inq.ExecuteScalar();
			Log::Trace("", __FUNCTION__, "cd_count	= [{0}]", cd_count);
			if (cd_count < 1)
			{
				Log::Trace("", __FUNCTION__, "巴拉巴拉1");
				sprintf(s.msg, heat_no + "改炉号责任单位不存在请确认！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq.Close();

			//查询违规类型
			CString	sqllx = "SELECT COUNT(*) FROM TEP0002 WHERE CODE_CLASS='QMWG02' AND CODE_DESC_1_CONTENT ='" + bcls_rec->Tables[0].Rows[i]["UNRULL_FLAG_LX"].ToString().Trim() + "'";
			Log::Trace("", __FUNCTION__, "sqllx	= [{0}]", (const char*)sqllx);
			cmd_inq.Parameters.Clear();
			cmd_inq.SetCommandText(sqllx);
			cd_count = cmd_inq.ExecuteScalar();
			if (cd_count < 1)
			{
				Log::Trace("", __FUNCTION__, "巴拉巴拉2");
				sprintf(s.msg, heat_no + "改炉号违规类型不存在请确认！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq.Close();


			//查询违规级别
			CString	sqljb = "SELECT COUNT(*) FROM TEP0002 WHERE CODE_CLASS='QMWG03' AND CODE_DESC_1_CONTENT ='" + bcls_rec->Tables[0].Rows[i]["UNRULL_FLAG_ID"].ToString().Trim() + "'";
			Log::Trace("", __FUNCTION__, "sqljb	= [{0}]", (const char*)sqljb);
			cmd_inq.Parameters.Clear();
			cmd_inq.SetCommandText(sqljb);
			cd_count = cmd_inq.ExecuteScalar();
			if (cd_count < 1)
			{
				Log::Trace("", __FUNCTION__, "巴拉巴拉3");
				sprintf(s.msg, heat_no + "改炉号违规级别不存在请确认！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq.Close();

			int seq_no = 0;
			//查询序列号最大
			CString sqlstr1 = "SELECT MAX(KEY_SEQ) FROM TQMTSWG ";
			cmd_inq.SetCommandText(sqlstr1);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				seq_no = cmd_inq.GetInt16(1);
			}
			cmd_inq.Close();

			Log::Trace("", "Count", bcls_rec->Tables[0].Rows.get_Count());
			tqmtswg["REC_CREATOR"] = s.userid;
			tqmtswg["REC_CREATE_TIME"] = datetime;
			tqmtswg["KEY_SEQ"] = seq_no + 1;
			tqmtswg.TrimOrBlank();
			tqmtswg.Delete("HEAT_NO");
			tqmtswg.Insert();


			//发送宝武聊天
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["HEAT_NO"] = tqmtswg["HEAT_NO"];
			Log::Trace("", "", "888,HEAT_NO=[{0}]", tqmtswg["HEAT_NO"].ToString());
			bcls_rec_s.Tables[0].Rows[0]["ST_NO"] = tqmtswg["ST_NO"];
			bcls_rec_s.Tables[0].Rows[0]["SHIFT_GROUP_BZ"] = tqmtswg["SHIFT_GROUP_BZ"];
			Log::Trace("", "", "SHIFT_GROUP_BZ=[{0}]", tqmtswg["SHIFT_GROUP_BZ"].ToString());
			bcls_rec_s.Tables[0].Rows[0]["SAP_ZRDW_DESC"] = tqmtswg["SAP_ZRDW_DESC"];
			bcls_rec_s.Tables[0].Rows[0]["UNRULL_FLAG_XM"] = tqmtswg["UNRULL_FLAG_XM"];
			Log::Trace("", "", "UNRULL_FLAG_XM=[{0}]", tqmtswg["UNRULL_FLAG_XM"].ToString());
			bcls_rec_s.Tables[0].Rows[0]["UNRULL_FLAG_LX"] = tqmtswg["UNRULL_FLAG_LX"];
			bcls_rec_s.Tables[0].Rows[0]["UNRULL_FLAG_ID"] = tqmtswg["UNRULL_FLAG_ID"];
			bcls_rec_s.Tables[0].Rows[0]["TRACE_WT_1_DESC"] = tqmtswg["TRACE_WT_1_DESC"];
			bcls_rec_s.Tables[0].Rows[0]["ERR_DESC"] = tqmtswg["ERR_DESC"];
			bcls_rec_s.Tables[0].Rows[0]["EXAMINER"] = tqmtswg["EXAMINER"];
			bcls_rec_s.Tables[0].Rows[0]["JC_RQ_TIME"] = tqmtswg["JC_RQ_TIME"];
			Log::Trace("", "", "JC_RQ_TIME=[{0}]", tqmtswg["JC_RQ_TIME"].ToString());


			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "8";
			bcls_rec_s.Tables[0].Rows[0]["REMARK"] = tqmtswg["HEAT_NO"];
			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
			if (doFlag < 0)
			{
			strcpy(s.msg, "调用函数报错!");
			throw CApplicationException(-1, s.msg, log.Location);
			}

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


