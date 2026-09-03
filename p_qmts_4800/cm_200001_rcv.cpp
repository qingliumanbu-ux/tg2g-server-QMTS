/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   刘家岩
Version:    1.0
Date:     2012-02-16
Description: 炉次确定电文接收
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
#include "epex.h"
 
/* ***** 外部函数申明 ***** */

//调用物料函数：更新板坯出钢记号 
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//调用炼钢函数更新最终出钢记号或者封锁
int f_cm_00x0qe_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//调用炉次成分发送程序

/*<remark>=========================================================
/// <summary>
/// 炉次确定电文接收
/// 电文号200001
/// 
/// <para>
/// 获取输入参数：制造命令号，预定出钢记号 ；
/// </para>
/// <para>数据库表：TQMTS29 实绩_炉次代表成分       
/// 数据库表：TMMSM01 炼钢板坯物料主表
/// 数据库表：TQMTQQ0 实绩表_原料化学成份
/// 数据库表：TQMTQB0 炉次信息表
/// </para>
/// </summary>
/// <param name="PONO">计划号    </param>
/// <param name="ST_NO">材料号    </param>
===========================================================</remark>*/  

// service入口
BM2F_ENTERACE_TELE(cm_200001_rcv)

int f_cm_200001_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{	
	
	CTracer log(__FUNCTION__);  

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i = 0;
	CString QMTSBlock;

	CDecimal v_count = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_du_flag = " ";
	int 	fetchRowCount = 0;
	struct  ei_sys s_tmp;
	int 	elm_ok_2 = 0;

	EIClass inBlock1;  //调用PM模块的接口
	EIClass bcls_rec_f;	//调用物料函数，置板坯决定/最终出钢记号
	EIClass bcls_ret_f;
	EIClass bcls_rec_send;	//调用电文发送函数
	EIClass bcls_ret_send;
	/* 实体类定义 */
	CModel tqmts29("TQMTS29");
	CModel tmmsm01("TMMSM01");
	CModel tqmtqq0("TQMTQQ0");
	CModel tqmtqb0("TQMTQB0");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//输入参数定义
		//1)设置PM接口的输入参数:
		//inBlock1.Tables[0].Columns.Add(DT_STRING, "pono");    //制造命令号
		//inBlock1.Tables[0].Columns.Add(DT_STRING, "st_no");    //炉次确定的出钢记号

		QMTSBlock = "MM0099";
		if (!bcls_rec_f.Tables.Contains("MM0099"))
		{
			bcls_rec_f.Tables.Add(QMTSBlock);
		}
		bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "MAT_KIND");		/*物料类型*/
		bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "EVENT_ID");		/*事件标识*/
		bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "EVENT_LINE_TYPE"); /*事件产线类型*/
		bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "SYSTEM_ID");		/*系统标识*/
		bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "FUNC_ID");			/*功能标识*/
		bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "MAT_NO");			/*材料号*/
		bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "FIN_ST_NO");		/*最终出钢记号*/
		bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "ST_NO");			/*出钢记号*/
		bcls_rec_f.Tables[QMTSBlock].Columns.Add(DT_STRING, "EVENT_DESC");		/*事件描述*/

		/* ***** 获取输入参数 ***** */
		strcpy(s.userid, "200001");

		/* ***** 获取主表部分数据 ***** */
		v_du_flag = bcls_rec->Tables[0].Rows[0]["du_flag"].ToString().Trim();
		tqmtqb0["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		tqmtqb0["PONO"] = bcls_rec->Tables[0].Rows[0]["pono"].ToString().Trim();
		tqmtqb0["ST_NO"] = bcls_rec->Tables[0].Rows[0]["deci_st_no"].ToString().Trim().TrimOrBlank();  //预定出钢记号
		tqmtqb0["PCH_JUDGE_CODE"] = bcls_rec->Tables[0].Rows[0]["judge_code"].ToString().Trim();

		/* ***** 打印输入参数 ***** */
		Log::Trace("", "", "du_flag[{0}]", v_du_flag);
		Log::Trace("", "", "heat_no[{0}]", tqmtqb0["HEAT_NO"].ToString());
		Log::Trace("", "", "pono[{0}]", tqmtqb0["PONO"].ToString());
		Log::Trace("", "", "deci_st_no[{0}]", tqmtqb0["ST_NO"].ToString());
		Log::Trace("", "", "judge_code[{0}]", tqmtqb0["PCH_JUDGE_CODE"].ToString());


		if (v_du_flag == "D")//代表成分选择取消
		{
			//删除熔炼成分信息
			tqmtqb0.Delete("HEAT_NO");

			tqmtqq0["HEAT_NO"] = tqmtqb0["HEAT_NO"];
			tqmtqq0.Delete("HEAT_NO");

			//已经选定最终出钢记号的要清空
			//抛物料跟踪
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default: // 所有数据库适用，通用SQL语句
				sqlstr = "   SELECT * "
					"  FROM TMMSM01 "
					" WHERE HEAT_NO = @HEAT_NO "
					"   AND FIN_ST_NO !=' '"
					" ORDER BY mat_no ASC ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			//压入绑定参数，""包括的第一个参数的名称与SQL语句中@后对应的变量完全一致，大小写敏感
			cmd_inq.Parameters.Set("HEAT_NO", tqmtqb0["HEAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			fetchRowCount = 0;
			bcls_rec_f.Tables[QMTSBlock].Rows.Clear();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tmmsm01);
				Log::Trace("", "", "当前板坯号是[{0}]", (const char*)tmmsm01["MAT_NO"].ToString());

				//调用物料函数，修正板坯决定、判定出钢记号（当flag为3时 值同最终出钢记号）
				//*********************************   5. 调用物料函数，修正板坯决定、最终出钢记号*********************************//



				Log::Trace("", "", "tmmsm01.FIN_ST_NO[{0}]", tmmsm01["FIN_ST_NO"].ToString());
				tmmsm01.MergeTo(bcls_rec_f.Tables[QMTSBlock], false);

				Log::Trace("", "", "tmmsm01.MAT_KIND[{0}]", tmmsm01["MAT_KIND"].ToString());

				bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["MAT_KIND"] = tmmsm01["MAT_KIND"];
				Log::Trace("", "", "2tmmsm01.MAT_KIND[{0}]", tmmsm01["MAT_KIND"].ToString());
				bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["EVENT_ID"] = "QM70";//生成板坯出钢记号
				bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["EVENT_LINE_TYPE"] = "00";
				bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["SYSTEM_ID"] = "QMTS";
				bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["FUNC_ID"] = "qmts25s_rep_unsel";
				Log::Trace("", "", "3tmmsm01.MAT_KIND[{0}]", tmmsm01["MAT_KIND"].ToString());
				bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["MAT_NO"] = tmmsm01["MAT_NO"];
				bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["FIN_ST_NO"] = " ";

				bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["ST_NO"] = tmmsm01["ST_NO"];
				Log::Trace("", "", "4tmmsm01.MAT_KIND[{0}]", tmmsm01["MAT_KIND"].ToString());
				bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["EVENT_DESC"] = "代表成分取消";
				fetchRowCount++;

			}
			cmd_inq.Close();
			Log::Trace("", "", "fetchRowCount[{0}]", fetchRowCount);
			/*if (fetchRowCount == 0)
			{
			strcpy(s.msg, "钢坯未切断，不能收成分实绩。");
			throw CApplicationException(-1, s.msg, log.Location);
			}*/
			//调用物料函数，合格：修正板坯决定、判定出钢记号，不合格：不更新最终出钢记号，只做封锁
			i = bcls_rec_f.Tables[QMTSBlock].Rows.get_Count();
			if (i > 0)
			{
				doFlag = f_mmsm99(&bcls_rec_f, &bcls_ret_f, conn);
				if (doFlag != 0)
				{
					Log::Trace("", "", "f_mmsm99() msg = [{0}]", s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
		else//代表成分选择
		{
			/* ***** 获取质量部分炉次代表成分数据 ***** */

			//写入炉次信息表
			Log::Trace("", "", "写入炉次信息表");

			tqmts29["HEAT_NO"] = tqmtqb0["HEAT_NO"];
			tqmts29.Delete("HEAT_NO");

			tqmtqb0.Delete("HEAT_NO");
			tqmtqb0["VENDOR_CODE"] = "system";
			tqmtqb0["REC_CREATOR"] = "xcom_pes";
			tqmtqb0["REC_CREATE_TIME"] = datetime;
			tqmtqb0.TrimOrBlank();
			tqmtqb0.Insert();

			tqmtqq0["HEAT_NO"] = tqmtqb0["HEAT_NO"];
			tqmtqq0.Delete("HEAT_NO");

			elm_ok_2 = 0;//不合格元素的数量，清零

			//对输入信息循环处理
			for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				//取得单行传入信息
				tqmts29.Reset();
				tqmts29.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tqmts29["PONO"] = bcls_rec->Tables[0].Rows[0]["pono"].ToString().TrimOrBlank();
				tqmts29["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().TrimOrBlank();
				tqmts29["ST_NO"] = bcls_rec->Tables[0].Rows[0]["deci_st_no"].ToString().TrimOrBlank();

				Log::Trace("", "", "tqmts29.ELM_CODE[{0}]", (const char*)tqmts29["ELM_CODE"].ToString());

				if (tqmts29["ELM_CODE"].ToString().TrimOrBlank() == " ")
				{
					break;
				}

				if (tqmts29["ELM_OK"].ToDecimal() != 8 && tqmts29["ELM_OK"].ToDecimal() != 9 && tqmts29["ELM_OK"].ToDecimal() != 3)//不合格
					elm_ok_2++;

				/******************** 赋初值 *************************/
				tqmts29["REC_CREATOR"] = "xcom_pes";
				tqmts29["REC_CREATE_TIME"] = datetime;
				tqmts29["REC_REVISOR"] = " ";
				tqmts29["REC_REVISE_TIME"] = " ";
				tqmts29["ARCHIVE_FLAG"] = " ";

				//写入炼钢炉次代表成分表
				Log::Trace("", "", "写入炼钢炉次代表成分表tqmts29");

				tqmts29.TrimOrBlank();
				tqmts29.Insert();

				//写入实绩表_原料化学成份
				Log::Trace("", "", "写入实绩表_原料化学成份TQMTQQ0");

				tqmtqq0["PONO"] = tqmtqb0["PONO"];
				tqmtqq0["HEAT_NO"] = tqmtqb0["HEAT_NO"];
				tqmtqq0["VENDOR_CODE"] = "system";
				tqmtqq0["ST_NO"] = tqmtqb0["ST_NO"];
				tqmtqq0["ELM_POS"] = tqmts29["ELM_POS"];
				tqmtqq0["ELM_CODE"] = tqmts29["ELM_CODE"];
				tqmtqq0["ELM_NAME"] = tqmts29["ELM_NAME"];
				tqmtqq0["ELM_UNIT"] = tqmts29["ELM_UNIT"];
				tqmtqq0["ELM_ACT"] = tqmts29["ELM_VALUE"];
				//tqmtqq0["PCH_JUDGE_CODE"] = tqmts29["ELM_OK"];
				tqmtqq0["REC_CREATOR"] = tqmts29["REC_CREATOR"];
				tqmtqq0["REC_CREATE_TIME"] = tqmts29["REC_CREATE_TIME"];

				tqmtqq0.TrimOrBlank();
				tqmtqq0.Insert();
			}
			Log::Trace("", "", "不合格元素有[{0}]个", elm_ok_2);
			if (elm_ok_2 > 0)
			{
				tqmtqb0["HEAT_NO"] = tqmtqb0["HEAT_NO"];
				tqmtqb0["PCH_JUDGE_CODE"] = "2";//不合格
				tqmtqb0.Update("PCH_JUDGE_CODE", "HEAT_NO");
			}
			int i_tp = CTransactionManager::Commit(0);
			if (i_tp < 0)
			{
				Log::Trace("", "", "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&i_tp[{0}]", i_tp);
				CTransactionManager::Abort(0);
			}
			CTransactionManager::Begin(0, 0);

			//炉次下的每个板坯，调物料跟踪函数
			fetchRowCount = 0;
			if (elm_ok_2 <= 0)
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
						"   FROM TMMSM01 "
						"  WHERE HEAT_NO = @heat_no "
						"    AND IF_TRANSFER != '1' "//交接坯标志，1表示交接坯，不自动置最终出钢记号，需人工判定
						;
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", tqmtqb0["HEAT_NO"].ToString());
				cmd_inq.ExecuteReader();

				while (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm01);

					Log::Trace("", "", "******************当前板坯号[{0}]", (const char*)tmmsm01["MAT_NO"].ToString());

					tmmsm01.MergeTo(bcls_rec_f.Tables[QMTSBlock], false);
					bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["MAT_KIND"] = tmmsm01["MAT_KIND"];
					bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["EVENT_ID"] = "QM70";//修改板坯上的最终出钢记号
					bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["EVENT_LINE_TYPE"] = "00";
					bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["SYSTEM_ID"] = "QMTS";
					bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["FUNC_ID"] = "cm_200001_rcv";
					bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["FIN_ST_NO"] = tqmtqb0["ST_NO"];//把预定出钢记号作为最终出钢记号
					bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["ST_NO"] = tqmtqb0["ST_NO"];
					bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["EVENT_DESC"] = "代表成分选定";
					fetchRowCount++;
				}
				cmd_inq.Close();
			}
			//20230918 注释qyb 保持3，4级状态一致
			//else if (elm_ok_2 > 0)//不合格：不更新最终出钢记号，只做质量封锁
			//{
			//	switch (conn->DatabaseKind)
			//	{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:						// 所有数据库适用，通用SQL语句
			//		sqlstr = " SELECT * "
			//			"   FROM TMMSM01 "
			//			"  WHERE HEAT_NO = @heat_no ";
			//		break;
			//	}
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.Parameters.Set("heat_no", tqmtqb0["HEAT_NO"].ToString());
			//	cmd_inq.ExecuteReader();
			//	while (cmd_inq.Read())
			//	{
			//		cmd_inq.Fetch(tmmsm01);

			//		Log::Trace("", "", "******************当前板坯号[{0}]", (const char*)tmmsm01["MAT_NO"].ToString());

			//		tmmsm01.MergeTo(bcls_rec_f.Tables[QMTSBlock], false);
			//		bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["MAT_KIND"] = tmmsm01["MAT_KIND"];
			//		bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["EVENT_ID"] = "QM17";//材料质量封锁
			//		bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["EVENT_LINE_TYPE"] = "00";
			//		bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["SYSTEM_ID"] = "QMTS";
			//		bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["FUNC_ID"] = "cm_200001_rcv";
			//		bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["MAT_NO"] = tmmsm01["MAT_NO"];
			//		bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["EVENT_DESC"] = "代表成分不合格";
			//		bcls_rec_f.Tables[QMTSBlock].Rows[fetchRowCount]["HOLD_CAUSE_CODE"] = "QM01";
			//		fetchRowCount++;
			//	}
			//	cmd_inq.Close();
			//}

			Log::Trace("", "", "当前炉次涉及的板坯块数[{0}]，非0时调物料函数", fetchRowCount);

			//调用物料函数
			i = bcls_rec_f.Tables[QMTSBlock].Rows.get_Count();
			if (i > 0)
			{
				doFlag = f_mmsm99(&bcls_rec_f, &bcls_ret_f, conn);
				if (doFlag != 0)
				{
					Log::Trace("", "", "f_mmsm99() msg = [{0}]", s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}//代表成分选择end


#ifdef _SYS_MMS    //2023.3.1 炼轧一体  不群发电文。
		//Log::Trace("", "", "**********调用炉次成分发送函数**********");
		//bcls_rec_send.Tables[0].Columns.Add(DT_STRING, "PONO");		/*制造命令号*/
		//bcls_rec_send.Tables[0].Rows.Add();
		//bcls_rec_send.Tables[0].Rows[0]["PONO"] = bcls_rec->Tables[0].Rows[0]["pono"].ToString().Trim();
		///*群发PES*/
		//doFlag = f_cm_00x0qe_snd(&bcls_rec_send, &bcls_ret_send, conn);
		//if (doFlag != 0)
		//{
		//	Log::Trace("", "", "f_cm_00x0qe_snd() msg = [{0}]", s.msg);
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
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
