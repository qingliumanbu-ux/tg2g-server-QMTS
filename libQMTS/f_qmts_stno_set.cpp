/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2022-11-14 19:54:28
Description: 代表成分选定，物料事件调用
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT

int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//调用炼钢函数更新最终出钢记号
int f_qmts_stno_set(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	CModel tqmts23("TQMTS23");

	//调用物料事件
	EIClass bcls_rec_sm;
	EIClass bcls_ret_sm;
	bcls_rec_sm.Tables[0].set_TableName("MM0099");
	bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");		/*事件标识*/
	bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");/*事件产线类型*/
	bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");		/*系统标识*/
	bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");		/*功能标识*/
	bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");			/*材料号*/
	bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "FIN_ST_NO");		/*最终出钢记号*/
	bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "ST_NO");			/*出钢记号*/
	bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "DECI_ST_NO");		/*决定出钢记号*/
	bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "JUDGE_ST_NO");		/*判定出钢记号*/
	bcls_rec_sm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_DESC");		/*事件描述*/
	try
	{
		//调用物料事件
		sqlstr = " SELECT MAT_NO, CUT_ST_NO,'TMMSM01' FROM tmmsm01 WHERE HEAT_NO = @HEAT_NO AND JUDGE_ST_NO = ' '";
		cmd.SetCommandText(sqlstr);
		tqmts23["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		cmd.Parameters.Set("HEAT_NO", bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank());
		cmd.ExecuteReader();
		int mat_count = 0;
		bcls_rec_sm.Tables["MM0099"].Rows.Clear();
		tqmts23.Query("HEAT_NO");
		while (cmd.Read()){
			bcls_rec_sm.Tables["MM0099"].Rows.Add();
			bcls_rec_sm.Tables["MM0099"].Rows[mat_count]["EVENT_ID"] = "QM70";//修改板坯上的最终出钢记号
			bcls_rec_sm.Tables["MM0099"].Rows[mat_count]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_sm.Tables["MM0099"].Rows[mat_count]["SYSTEM_ID"] = "QMTS";
			bcls_rec_sm.Tables["MM0099"].Rows[mat_count]["FUNC_ID"] = s.svc_name;
			bcls_rec_sm.Tables["MM0099"].Rows[mat_count]["MAT_NO"] = cmd.GetString(1);
			bcls_rec_sm.Tables["MM0099"].Rows[mat_count]["FIN_ST_NO"] = tqmts23["FIN_ST_NO"].ToString();
			bcls_rec_sm.Tables["MM0099"].Rows[mat_count]["ST_NO"] = tqmts23["ST_NO"];
			bcls_rec_sm.Tables["MM0099"].Rows[mat_count]["DECI_ST_NO"] = tqmts23["DECI_ST_NO"];
			bcls_rec_sm.Tables["MM0099"].Rows[mat_count]["JUDGE_ST_NO"] = tqmts23["JUDGE_ST_NO"];
			if (tqmts23["JUDGE_ST_NO"].ToString().Trim() != ""){
				bcls_rec_sm.Tables["MM0099"].Rows[mat_count]["EVENT_DESC"] = "炉次终判";
			}
			else
			{
				bcls_rec_sm.Tables["MM0099"].Rows[mat_count]["EVENT_DESC"] = "炉次自动判定";
			}
			mat_count++;
		}
		if (mat_count != 0){
			Log::Trace("", "", "line = {0}", __LINE__);
			doFlag = f_mmsm99(&bcls_rec_sm, &bcls_ret_sm, conn);

			if (doFlag != 0){
				Log::Trace("", "", "f_mmsm99() msg = [{0}]", s.msg);
				s.flag = 0;
				doFlag = 0;
				throw CApplicationException(-1, s.msg, log.Location);
			}
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


