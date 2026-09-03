/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      guxia
Version:     1.0
Date:        2015-07-03
Description: 修改板坯最终出钢记号
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中




BM2_FUNCTION_IMPORT
int f_qm200002_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
//向热轧PES发送板坯钢种判定信息(PES的物料模块接收)

//调用物料函数：更新板坯出钢记号
BM2_FUNCTION_IMPORT
//int f_mmsm_qm71(EIClass *bcls_rec,EIClass *bcls_ret, CDbConnection *conn);  //炉次改判时更新板坯的出钢记号,同时封锁/释放
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//调用炼钢函数更行最终出钢记号
BM2_FUNCTION_EXPORT
int f_qmts_jud_01(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	int mat_count = 0;

	CString s_pono = " ";				//制造命令号
	CString s_st_no = " ";			//传入炉次出钢记号
	CString s_heat_no = " ";
	CString sqlstr = "";

	//调用物料函数，置板坯决定/最终出钢记号、对板坯做封锁、释放
	EIClass bcls_rec_mm;
	EIClass bcls_ret_mm;
	bcls_rec_mm.Tables.Add("MM0099");
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");//事件号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");//板坯号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "PONO");//制造命令号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");//功能标识
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");//系统标识
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "ST_NO");//当前出钢记号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "FIN_ST_NO");//最终出钢记号
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "JUDGE_MAKER");//判定责任者
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "JUDGE_TIME");//判定时刻
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_INT16, "CALL_FLAG");//调用标记
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "HOLD_FLAG");//封锁标记(0: 释放 1：管理封锁 2：质量封锁； 3: 管理封锁+质量封锁)
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_DESC");//事件描述
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "REL_TIME");//释放时刻
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "REL_MAKER");//释放责任者
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "REL_REMARK");//释放注释
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "HOLD_TIME");///封锁时刻
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "HOLD_MAKER");//封锁责任者
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "HOLD_CAUSE_CODE");//封锁原因代码
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "HOLD_REMARK");//封锁注释
	bcls_rec_mm.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");//事件产线类型

	//调用物料电文函数，置板坯决定/最终出钢记号、对板坯做封锁、释放
	EIClass bcls_rec_200002;
	EIClass bcls_ret_200002;
	bcls_rec_200002.Tables.Add("MMSMSM");

	bcls_rec_200002.Tables["MMSMSM"].Columns.Add(DT_STRING, "MAT_NO");//板坯号
	bcls_rec_200002.Tables["MMSMSM"].Columns.Add(DT_STRING, "FIN_ST_NO");//最终出钢记号

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//*************************************************  1. 获取传入参数  *************************************************//
		s_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();	//制造命令号
		s_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();	//炉次决定/最终出钢记号
		s_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();	//制造命令号

		Log::Trace("", __FUNCTION__, "f_qmts_jud_01 IN:---pono = [{0}]s_heat_no[{1}]", s_pono, s_heat_no);
		Log::Trace("", __FUNCTION__, "f_qmts_jud_01 IN:---st_no = [{0}]", s_st_no);

		//*********************************************  2.修改板坯最终出钢记号  *********************************************//
		bcls_rec_200002.Tables["MMSMSM"].Rows.Clear();
		////////////////////////成分合格调用炼钢物料函数update by yiling 
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TMMSM01 "
				" WHERE HEAT_NO = @s_heat_no "
				" ORDER BY MAT_NO ASC";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("s_heat_no", s_heat_no);
		cmd_inq.ExecuteReader();
		bcls_rec_mm.Tables["MM0099"].Rows.Clear();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmmsm01);

			tmmsm01.MergeTo(bcls_rec_mm.Tables["MM0099"], false);

			bcls_rec_mm.Tables["MM0099"].Rows[mat_count]["MAT_KIND"] = tmmsm01["MAT_KIND"];
			bcls_rec_mm.Tables["MM0099"].Rows[mat_count]["EVENT_ID"] = "QM70"; //生成板坯出钢记号
			bcls_rec_mm.Tables["MM0099"].Rows[mat_count]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_mm.Tables["MM0099"].Rows[mat_count]["SYSTEM_ID"] = "QMTS";
			bcls_rec_mm.Tables["MM0099"].Rows[mat_count]["FUNC_ID"] = "f_qmts_jud_01";
			bcls_rec_mm.Tables["MM0099"].Rows[mat_count]["MAT_NO"] = tmmsm01["MAT_NO"];
			bcls_rec_mm.Tables["MM0099"].Rows[mat_count]["FIN_ST_NO"] = s_st_no;
			bcls_rec_mm.Tables["MM0099"].Rows[mat_count]["ST_NO"] = s_st_no;
			bcls_rec_mm.Tables["MM0099"].Rows[mat_count]["EVENT_DESC"] = "炉次判综";

			bcls_rec_200002.Tables["MMSMSM"].Rows.Add();
			bcls_rec_200002.Tables["MMSMSM"].Rows[mat_count]["MAT_NO"] = tmmsm01["MAT_NO"];
			bcls_rec_200002.Tables["MMSMSM"].Rows[mat_count]["FIN_ST_NO"] = s_st_no;

			mat_count++;
		}
		cmd_inq.Close();

		Log::Trace("", "", "s_st_no= [{0}]mat_count[{1}]", s_st_no, mat_count);
		Log::Trace("", "", "bcls_rec_mm.Rows.Count= [{0}]", bcls_rec_mm.Tables["MM0099"].Rows.get_Count());

		if (mat_count != 0)
		{
			doFlag = f_mmsm99(&bcls_rec_mm, &bcls_ret_mm, conn);
			if (doFlag != 0)
			{
				strcat(s.msg, _RES("调用f_mmsm99出错。"));
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		

#ifdef _SYS_PES 
		//炉次确定时下达更新板坯的出钢记号电文
		doFlag = f_qm200002_snd(&bcls_rec_200002, &bcls_ret_200002, conn);
		if (doFlag != 0)
		{
			Log::Trace("", __FUNCTION__, "f_qm200002_snd() msg = [{0}]", s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}
#endif
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

