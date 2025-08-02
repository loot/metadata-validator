use std::process::Command;

const ENV_VAR_NAME: &str = "LMV_REVISION";

fn main() {
    if std::env::var_os(ENV_VAR_NAME).is_some() {
        return;
    }

    println!("cargo:rerun-if-changed=.git/HEAD");
    println!("cargo:rerun-if-changed=build.rs");

    let Ok(git_rev_parse_output) = Command::new("git")
        .args(["rev-parse", "--short", "HEAD"])
        .output()
    else {
        println!("cargo:warning=Calling git rev-parse failed, {ENV_VAR_NAME} will be unset!");
        return;
    };

    if let Ok(git_hash) = String::from_utf8(git_rev_parse_output.stdout) {
        println!("cargo:rustc-env={ENV_VAR_NAME}={git_hash}");
    } else {
        println!(
            "cargo:warning=Could not convert git rev-parse output to a UTF-8 string, {ENV_VAR_NAME} will be unset!"
        );
    }

    // Don't warn if this errors, as it will error if HEAD points directly to a commit.
    if let Ok(git_symbolic_ref_output) = Command::new("git").args(["symbolic-ref", "HEAD"]).output()
    {
        if let Ok(symbolic_ref) = String::from_utf8(git_symbolic_ref_output.stdout) {
            println!("cargo:rerun-if-changed=.git/{symbolic_ref}");
        } else {
            println!(
                "cargo:warning=Could not convert git symbolic-ref output to a UTF-8 string, {ENV_VAR_NAME} may become stale!"
            );
        }
    }
}
