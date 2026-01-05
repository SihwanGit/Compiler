// AST.java
// AST for S
package Lab02;

import java.util.*;

class Indent {
    public static void display(int level, String s) {
        String tab = "";
        System.out.println();
        for (int i=0; i<level; i++) //level만큼 공백문자 삽입
            tab = tab + "  ";
        System.out.print(tab + s); 
   }
} 

abstract class Command {
    // Command = Decl | Function | Stmt
    Type type =Type.UNDEF;
    public void display(int l) {  }
}

class Decls extends ArrayList<Decl> {
    // Decls = Decl*

    Decls() { super(); };
    Decls(Decl d) {
	    this.add(d);
    }
    // TODO: [Insert the code of display()]
	public void display(int level){
		Indent.display(level, "Decls");
		for(Decl decl: this) { //decl의 수만큼
			decl.display(level+1);
		}
	}    
}

class Decl extends Command {
    // Decl = Type type; Identifier id 
    Identifier id;
    Expr expr = null;
    int arraysize = 0;

    Decl (String s, Type t) {
        id = new Identifier(s); type = t;
    } // declaration 

    Decl (String s, Type t, int n) {
        id = new Identifier(s); type = t; arraysize = n;
    } // array declaration 

    Decl (String s, Type t, Expr e) {
        id = new Identifier(s); type = t; expr = e;
    } // declaration
   
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	Indent.display(level, "Decl");
    	type.display(level+1);
    	id.display(level+1);
    	if(expr != null) { //expr이 있다면 expr도 출력
    		expr.display(level+1);
    	}
    }
}

class Functions extends ArrayList<Function> {
    // Functions = Function*
}

class Function extends Command  {
    // Function = Type type; Identifier id; Decls params; Stmt stmt
    Identifier id;
    Decls params;
    Stmt stmt;
   
    Function(String s, Type t) { 
        id = new Identifier(s); type = t; params = null; stmt = null;
    }

    public String toString ( ) { 
       return id.toString()+params.toString(); 
    }
}

class Type {
    // Type = int | bool | string | fun | array | except | void
    final static Type INT = new Type("int");
    final static Type BOOL = new Type("bool");
    final static Type STRING = new Type("string");
    final static Type VOID = new Type("void");
    final static Type FUN = new Type("fun");
    final static Type ARRAY = new Type("array");
    final static Type EXC = new Type("exc");
    final static Type RAISEDEXC = new Type("raisedexc");
    final static Type UNDEF = new Type("undef");
    final static Type ERROR = new Type("error");
    
    protected String id;
    protected Type(String s) { id = s; }
    public String toString ( ) { return id; }
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	Indent.display(level, "Type: "+ id); //id가 타입의 문자열을 저장한 객체
    }
}

class ProtoType extends Type {
   // defines the type of a function and its parameters
   Type result;  
   Decls params;
   ProtoType (Type t, Decls ds) {
      super(t.id);
      result = t;
      params = ds;
   }
}

abstract class Stmt extends Command {
    // Stmt = Empty | Stmts | Assignment | If  | While | Let | Read | Print
}

class Empty extends Stmt {
    public void display (int level) {
        Indent.display(level, "Empty");
     }
}

class Stmts extends Stmt {
    // Stmts = Stmt*
    public ArrayList<Stmt> stmts = new ArrayList<Stmt>();
    
    Stmts() {
	    super(); 
    }

    Stmts(Stmt s) {
	     stmts.add(s);
    }
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	if (stmts != null && !stmts.isEmpty()) { // null과 비어 있는 리스트 확인
    		Indent.display(level, "Stmts");
    		for (Stmt stmt : stmts) {
    			stmt.display(level+1); // 각 Stmt의 display 메서드 호출
    	    }
    	} else {
    	    Indent.display(level+1, "No statements"); // 비어 있을 경우 메시지 출력
    	}
    }
}

class Assignment extends Stmt {
    // Assignment = Identifier id; Expr expr
    Identifier id;
    Array ar = null;
    Expr expr;

    Assignment (Identifier t, Expr e) {
        id = t;
        expr = e;
    }

    Assignment (Array a, Expr e) {
        ar = a;
        expr = e;
    }
    
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	Indent.display(level, "Assignment");
    	id.display(level+1);
    	expr.display(level+1);
    }
}

class If extends Stmt {
    // If = Expr expr; Stmt stmt1, stmt2;
    Expr expr;
    Stmt stmt1, stmt2;
    
    If (Expr t, Stmt tp) {
        expr = t; stmt1 = tp; stmt2 = new Empty( );
    }
    
    If (Expr t, Stmt tp, Stmt ep) {
        expr = t; stmt1 = tp; stmt2 = ep; 
    }
    
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	Indent.display(level, "If");
    	expr.display(level+1);
    	stmt1.display(level+1);
    	if(stmt2 != null) {
    		stmt2.display(level+1);
    	}
    }
}

class While extends Stmt {
    // While = Expr expr; Stmt stmt;
    Expr expr;
    Stmt stmt;

    While (Expr t, Stmt b) {
        expr = t; stmt = b;
    }
    
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	Indent.display(level, "While");
    	expr.display(level+1);
    	stmt.display(level+1);
    }
}

class Let extends Stmt {
    // Let = Decls decls; Functions funs; Stmts stmts; // <- Disregard [Functions funs]
    Decls decls;
    Functions funs;
    Stmts stmts;

    Let(Decls ds, Stmts ss) {
        decls = ds;
		funs = null;
        stmts = ss;
    }

    Let(Decls ds, Functions fs, Stmts ss) {
        decls = ds;
	    funs = fs;
        stmts = ss;
    }
    
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	Indent.display(level, "Let");
    	decls.display(level+1);
/*    	if(funs != null) {
    		funs.display(level+1); //함수는 주석처리
    	}*/
    	stmts.display(level+1);
    }
}

class Read extends Stmt {
    // Read = Identifier id
    Identifier id;

    Read (Identifier v) {
        id = v;
    }
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	Indent.display(level, "Read");
        if (id != null) {
            id.display(level + 1);  // id가 있을 때
        } else {// id가 null
            Indent.display(level + 1, "<null identifier>");  // null인 경우 처리
        }
    }
}

class Print extends Stmt {
    // Print =  Expr expr
    Expr expr;

    Print (Expr e) {
        expr = e;
    }
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	Indent.display(level, "Print");

        // expr가 null일 때의 처리
        if (expr != null) {
            expr.display(level + 1);  // 들여쓰기 레벨 증가 후 표현식 출력
        } else {
            Indent.display(level + 1, "<null expression>");  // null인 경우 처리
        }
    }
}

class Return extends Stmt {
    Identifier fid;
    Expr expr;

    Return (String s, Expr e) {
        fid = new Identifier(s);
        expr = e;
    }
}

class Try extends Stmt {
    // Try = Identifier id; Stmt stmt1; Stmt stmt2; 
    Identifier eid;
    Stmt stmt1; 
    Stmt stmt2; 

    Try(Identifier id, Stmt s1, Stmt s2) {
        eid = id; 
        stmt1 = s1;
        stmt2 = s2;
    }
}

class Raise extends Stmt {
    Identifier eid;

    Raise(Identifier id) {
        eid = id;
    }
}

class Exprs extends ArrayList<Expr> {
    // Exprs = Expr*
}

abstract class Expr extends Stmt {
    // Expr = Identifier | Value | Binary | Unary | Call

}

class Call extends Expr { 
    Identifier fid;  
    Exprs args;

    Call(Identifier id, Exprs a) {
       fid = id;
       args = a;
    }
}

class Identifier extends Expr {
    // Identifier = String id
    private String id;

    Identifier(String s) { id = s; }

    public String toString( ) { return id; }
    
    public boolean equals (Object obj) {
        String s = ((Identifier) obj).id;
        return id.equals(s);
    }
    
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	if (id != null) {
            Indent.display(level, "Identifier: " + id); // id를 출력
        } else {
            Indent.display(level, "Identifier: <null>"); // id가 null일 경우
        }
    }
}

class Array extends Expr {
    // Array = Identifier id; Expr expr
    Identifier id;
    Expr expr = null;

    Array(Identifier s, Expr e) {id = s; expr = e;}

    public String toString( ) { return id.toString(); }
    
    public boolean equals (Object obj) {
        String s = ((Array) obj).id.toString();
        return id.equals(s);
    }
}

class Value extends Expr {
    // Value = int | bool | string | array | function 
    protected boolean undef = true;
    Object value = null; // Type type;
    
    Value(Type t) {
        type = t;  
        if (type == Type.INT) value = new Integer(0);
        if (type == Type.BOOL) value = new Boolean(false);
        if (type == Type.STRING) value = "";
        undef = false;
    }

    Value(Object v) {
        if (v instanceof Integer) type = Type.INT;
        if (v instanceof Boolean) type = Type.BOOL;
        if (v instanceof String) type = Type.STRING;
        if (v instanceof Function) type = Type.FUN;
        if (v instanceof Value[]) type = Type.ARRAY;
        value = v; undef = false; 
    }

    Object value() { return value; }

    int intValue( ) { 
        if (value instanceof Integer) 
            return ((Integer) value).intValue(); 
        else return 0;
    }
    
    boolean boolValue( ) { 
        if (value instanceof Boolean) 
            return ((Boolean) value).booleanValue(); 
        else return false;
    } 

    String stringValue ( ) {
        if (value instanceof String) 
            return (String) value; 
        else return "";
    }

    Function funValue ( ) {
        if (value instanceof Function) 
            return (Function) value; 
        else return null;
    }

    Value[] arrValue ( ) {
        if (value instanceof Value[]) 
            return (Value[]) value; 
        else return null;
    }

    Type type ( ) { return type; }

    public String toString( ) {
        //if (undef) return "undef";
        if (type == Type.INT) return "" + intValue(); 
        if (type == Type.BOOL) return "" + boolValue();
	    if (type == Type.STRING) return "" + stringValue();
        if (type == Type.FUN) return "" + funValue();
        if (type == Type.ARRAY) return "" + arrValue();
        return "undef";
    }
    
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	if (value != null) {
            Indent.display(level, "Value: " + value); // value를 출력
        } else {
            Indent.display(level, "Value: <null>"); // value가 null일 경우
        }
    }
}

class Binary extends Expr {
// Binary = Operator op; Expr expr1; Expr expr2;
    Operator op;
    Expr expr1, expr2;

    Binary (Operator o, Expr e1, Expr e2) {
        op = o; expr1 = e1; expr2 = e2;
    } // binary
    
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	Indent.display(level, "Binary");
    	op.display(level+1);
    	expr1.display(level+1);
    	expr2.display(level+1);
    }
}

class Unary extends Expr {
    // Unary = Operator op; Expr expr
    Operator op;
    Expr expr;

    Unary (Operator o, Expr e) {
        op = o; //(o.val == "-") ? new Operator("neg"): o; 
        expr = e;
    } // unary
    
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	Indent.display(level, "Unary");
    	op.display(level+1);
    	expr.display(level+1);
    }
}

class Operator {
    String val; //val이 연산자
    
    Operator (String s) { 
	val = s; 
    }

    public String toString( ) { 
	return val; 
    }

    public boolean equals(Object obj) { 
	return val.equals(obj); 
    }
    
    // TODO: [Insert the code of display()]
    public void display(int level) {
    	Indent.display(level, "Operator: " + val);
    }
}