//Dec 16 08:33//请不要修改此行

#include "stdafx.h"

BM2F_ENTERACE(qmtiz1_dtl);

int f_qmtiz1_dtl(EIClass * bcls_rec,EIClass * bcls_ret,CDbConnection * conn)
{
    //系统日志类定义
	CTracer log(__FUNCTION__);	
	
	//程序用变量
	int doFlag=0;	
	CString q_mat_no=" ";
	CString sqlstr = "";
	
	try
	{
	    //接收从前台传入的值
		q_mat_no  =bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
				
		sqlstr=" SELECT ITEM_CNAME,ITEM_VALUE,ITEM_UPPER_VALUE,ITEM_LOWER_VALUE,SAMPLE_LOT_NO "
		               " FROM TQMTIZ2 "
					   " WHERE MAT_NO = @mat_no "
					   " ORDER BY SEQ_NO ASC"
					   ;
	
        //压入绑定参数，""包括的第一个参数的名称与SQL语句中@后对应的变量完全一致，大小写敏感
        CDbCommand cmd(sqlstr,conn);
		//传递参数进SQL语句
        cmd.Parameters.Set("mat_no",q_mat_no); 	
		int count=cmd.ExecuteQuery(bcls_ret->Tables[0],0,-1);
		bcls_ret->Tables[0].set_TableName("QMTIZ1_INQD");
        Log::Trace("QMTI","qmtiz1_dtl","共查询了{0}条数据",count);
        /*.print();*/
		sprintf(s.msg,"查询成功！");			
	
	}
	
	//捕获数据库操作异常
	catch(CDbException& ex)  
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}

	//try块程序发生异常
	catch(const CApplicationException& ex)
	{
		doFlag = ex.GetCode();
		sprintf(s.msg,"%s:%s",(const char*)ex.GetSource(),(const char*)ex.GetMsg());
	}

	//平台异常
	catch(const CException& ex)
	{
		doFlag = -1;
		strcpy(s.msg, (const char*)ex.GetMsg());
	}

	s.flag = doFlag;

	return(doFlag);
}