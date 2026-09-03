/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-01-15
Description: 板坯待判处置判定审核
**************************************************/
//框架头文件
#include "stdafx.h"

//业务头文件

//外部函数声明

BM2F_ENTERACE(qmbsm5_qmts30_add)

int f_qmbsm5_qmts30_add(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	/* 业务变量 */
	CModel tqmts30("TQMTS30");
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	double mat_wt;
	int mat_nun = 0;
	CString heat_no = " ";
	CString rcv_mat_flag = " ";
	try
	{
		CString full_furnace = bcls_rec->Tables[1].Rows[0]["FULL_FURNACE"].ToString().Trim();
		Log::Trace("", "", "新增开始,FULL_FURNACE=[{0}]", full_furnace);
		CString reason = bcls_rec->Tables[1].Rows[0]["REASON"].ToString().Trim();
		Log::Trace("", "", "新增开始,REASON=[{0}]", reason);
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//整炉则材料号为空
			if (full_furnace.TrimOrBlank() == "是")
			{
				//清空太钢特有处置表
				tqmts30["HEAT_NO"] = bcls_rec->Tables[0].Rows[i]["HEAT_NO"];
				tqmts30["AREA"] = "北区";
				tqmts30["MAT_NO"] = " ";
				tqmts30.Query();
				tqmts30.Delete("HEAT_NO,AREA,MAT_NO");


				//每次循环先将数据清空 在merge数据
				tqmts30.Reset();
				tqmts30.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				Log::Trace("", "", "111");
				tqmts30["FULL_FURNACE"] = "是";
				tqmts30["MAT_NO"] = " ";
				//查询块数 重量
				sqlstr = "select count(MAT_NUM), sum(MAT_WT) from tmmsm01 where HEAT_NO = '" + bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString() + "' ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					mat_nun = cmd_inq.GetInt16(1);
					mat_wt = cmd_inq.GetDouble(2);
				}
				cmd_inq.Close();
				tqmts30["MAT_NUM"] = mat_nun;
				tqmts30["MAT_THEORY_WT"] = mat_wt;

				
				tqmts30["REASON_1"] = reason;//原因
				sqlstr = " SELECT SAMPLE_TAKEN_TIME  FROM TQMTS24 WHERE ST_SAMPLE_NO = ("
					"				SELECT ST_SAMPLE_NO  FROM TQMTSB0 WHERE HEAT_NO = @heat_no )";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmts30["PROD_DATE"] = cmd_inq.GetString(1).Substring(0, 8);
				}

			}
			else if (full_furnace.TrimOrBlank() == "否")
			{
				//查询是否收货  没有则提示报错
				sqlstr = "SELECT RCV_MAT_FLAG FROM TMMSM01 WHERE MAT_NO='" + bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString() + "' AND HEAT_NO='" + bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString() + "' AND (RCV_MAT_FLAG='N' OR RCV_MAT_FLAG='0') ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					rcv_mat_flag = cmd_inq.GetString(1);
				}
				if (rcv_mat_flag == "N" || rcv_mat_flag == "0")
				{
					strcpy(s.msg, _RES("该材料号没有收货不可处置")/*该材料号没有收货不可处置。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}


				tqmts30["HEAT_NO"] = bcls_rec->Tables[0].Rows[i]["HEAT_NO"];
				tqmts30["AREA"] = "北区";
				tqmts30["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"];
				tqmts30.Query();
				tqmts30.Delete("HEAT_NO,AREA,MAT_NO");


				//每次循环先将数据清空 在merge数据
				tqmts30.Reset();
				tqmts30.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				Log::Trace("", "", "1234");
				tqmts30["FULL_FURNACE"] = "否";
				//查询块数 重量
				sqlstr = "select MAT_NUM, MAT_WT from tmmsm01 where HEAT_NO = '" + bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString() + "' AND MAT_NO='" + bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString() + "'";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					mat_nun = cmd_inq.GetInt16(1);
					mat_wt = cmd_inq.GetDouble(2);
				}
				cmd_inq.Close();
				tqmts30["MAT_NUM"] = mat_nun;
				tqmts30["MAT_THEORY_WT"] = mat_wt;
				tqmts30["REASON"] = reason;//原因
				sqlstr = " SELECT SLAB_CUT_TIME FROM TMMSM01 WHERE MAT_NO = @mat_no ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_no", bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmts30["PROD_DATE"] = cmd_inq.GetString(1).Substring(0, 8);
				}
			}
			tqmts30["REPORT_TIME"] = datetime;
			tqmts30["REC_CREATOR"] = s.userid;
			tqmts30["REC_CREATE_TIME"] = datetime;
			tqmts30["REC_REVISOR"] = " ";
			tqmts30["REC_REVISE_TIME"] = " ";
			tqmts30["DECIDER"] = s.username;
			tqmts30["AREA"] = "北区";
			tqmts30["STATUS_FLAG"] = "0";//判定状态 未判
			tqmts30["INITIAL_STEEL"] = " ";//原钢种
			tqmts30["REMARK_3"] = "0";//成品处置

			tqmts30.TrimOrBlank();
			tqmts30.Insert();

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


