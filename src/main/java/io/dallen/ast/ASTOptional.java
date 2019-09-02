package io.dallen.ast;

public class ASTOptional<T extends AST.Statement> {
    T dat;

    private ASTOptional(T dat) {
        super();
        this.dat = dat;
    }

    public static <T extends AST.Statement> ASTOptional<T> of(T dat) {
        if(dat == null) {
            throw new UnsupportedOperationException("ASTOptional cannot be of null");
        }
        return new ASTOptional<>(dat);
    }

    public static <T extends AST.Statement> ASTOptional<T> empty() {
        return new ASTOptional<>(null);
    }

    public boolean isPresent() {
        return dat != null;
    }

    public T get() {
        return dat;
    }

    @Override
    public String toString() {
        if(dat == null) {
            return "ASTOptional.empty";
        } else {
            return "ASTOptional.of(" + dat.toString() + ")";
        }
    }

    public String toFlatString() {
        if(dat == null) {
            return "ASTOptional.empty";
        } else {
            return "ASTOptional.of(" + dat.toFlatString() + ")";
        }
    }
}
