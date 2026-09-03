/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      王淑玲
Version:     1.0
Date:        2023-14-14 14:08:16
Description: 表判TMMSM01信息
**************************************************/

#include "stdafx.h"
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//物料修改函数
int f_t82302_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(qm_mmsm01_bp)

int f_qm_mmsm01_bp(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		//调用物料事件 EICLASS
		EIClass bcls_rec_sm;
		EIClass bcls_ret_sm;
		bcls_rec_sm.Tables[0].set_TableName("MM0099");
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");		/*事件标识*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");/*事件产线类型*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");		/*系统标识*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");		/*功能标识*/
		bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");			/*材料号*/

		if (bcls_rec->Tables[0].Rows.get_Count() == 0)
		{
			return 0;
		}
		sqlstr = " SELECT MAT_NO, MAT_STATUS FROM tmmsm01 WHERE MAT_NO IN(";

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			sqlstr += "'";
			sqlstr += bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString();
			sqlstr += "',";
		}
		sqlstr = sqlstr.Substring(0, sqlstr.GetLength() - 1);
		sqlstr += ")";
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteQuery(bcls_rec->Tables[0]);
		Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", bcls_rec->Tables.get_Count());
		//获取前台上传的列
		for (int i = 0; i < bcls_rec->Tables[1].Columns.get_Count(); i++)
		{
			bcls_rec_sm.Tables["MM0099"].Columns.Add(bcls_rec->Tables[1].Columns[i].get_DataType(), bcls_rec->Tables[1].Columns[i].get_ColumnName());
		}
		Log::Info("", __FUNCTION__, "f_mmsm10_trace_linke   =[{0}]", __LINE__);
		Log::Info("", __FUNCTION__, "bcls_rec->Tables[0].Rows.get_Count()   =[{0}]", bcls_rec->Tables[0].Rows.get_Count());
		//添加数据项目
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			/*这里可以判定是否需要跳过或者报错*/

			bcls_rec_sm.Tables["MM0099"].Rows.Add();
			bcls_rec_sm.Tables["MM0099"].Rows[i].Merge(bcls_rec->Tables[1].Rows[0]);//获取所有列值
			if (!bcls_rec_sm.Tables["MM0099"].Columns.Contains("SURFACE_DECIDE_CODE"))
				bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "SURFACE_DECIDE_CODE");
			//bcls_rec_sm.Tables["MM0099"].Rows[i]["SURFACE_DECIDE_CODE"] = "1";
			bcls_rec_sm.Tables["MM0099"].Rows[i]["EVENT_ID"] = "QM20";//修改板坯上的最终出钢记号
			bcls_rec_sm.Tables["MM0099"].Rows[i]["EVENT_LINE_TYPE"] = "SM";
			bcls_rec_sm.Tables["MM0099"].Rows[i]["SYSTEM_ID"] = "QMTS";
			bcls_rec_sm.Tables["MM0099"].Rows[i]["FUNC_ID"] = s.svc_name;
			bcls_rec_sm.Tables["MM0099"].Rows[i]["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"];
		}
		Log::Info("", __FUNCTION__, "bcls_rec_sm.Tables[0].Rows.get_Count()   =[{0}]", bcls_rec_sm.Tables[0].Rows.get_Count());
		if (bcls_rec_sm.Tables[0].Rows.get_Count() == 0)
		{
			Log::Trace("", "", "没有需要判定的数据");
			return 0;
		}
		doFlag = f_mmsm99(&bcls_rec_sm, &bcls_ret_sm, conn);

		if (doFlag != 0){
			Log::Trace("", "", "f_mmsm99() msg = [{0}]", s.msg);
			s.flag = -1;
			doFlag = -1;
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (!bcls_rec->Tables[0].Columns.Contains("T82302"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "T82302");
		}
		bcls_rec->Tables[0].Rows[0]["T82302"] = "1";
		//发送智慧质量电文
		doFlag = f_t82302_snd(bcls_rec, bcls_ret, conn);

		if (doFlag != 0){
			Log::Trace("", "", "f_t82302_snd() msg = [{0}]", s.msg);
			s.flag = -1;
			doFlag = -1;
			throw CApplicationException(-1, s.msg, log.Location);
		}

	}
	catch (CDbException &ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char *)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException &ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException &ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
